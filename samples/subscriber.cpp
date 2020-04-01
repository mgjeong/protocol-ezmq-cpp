/*******************************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

#include <iostream>
#include <signal.h>
#include <condition_variable>
#include <memory>
#ifdef __linux__
#include <unistd.h>
#endif
#if defined(_WIN32)
#include <signal.h>
#endif

#include "EZMQAPI.h"
#include "EZMQSubscriber.h"
#include "EZMQErrorCodes.h"
#include "Event.pb.h"
#include "EZMQMessage.h"
#include "EZMQByteData.h"
#include "EZMQException.h"

using namespace std;
using namespace ezmq;

EZMQSubscriber *gSubscriber = nullptr ;
std::string gServerPublicKey = "tXJx&1^QE2g7WCXbF.$$TVP.wCtxwNhR8?iLi&S<";
std::string gClientPublicKey = "-QW?Ved(f:<::3d5tJ$[4Er&]6#9yr=vha/caBc(";
std::string gClientSecretKey = "ZB1@RS6Kv^zucova$kH(!o>tZCQ.<!Q)6-0aWFmW";

bool gIsStarted;
std::mutex gMutex;
std::condition_variable gCV;

void printEvent(const ezmq::Event &event)
{
    cout<<"--------------------------------------"<<endl;
    cout<<"Device:  " << event.device()<<endl;
    cout<<"Readings: "<<endl;

    int size = event.reading_size();
    int i=0;
    while (i<size)
    {
        Reading reading = event.reading(i);
        cout<<"Key: " + reading.name()<<endl;
        cout<<"Value: " + reading.value()<<endl;
        i++;
    }
    cout<<"----------------------------------------"<<endl;
}

void printByteData(const ezmq::EZMQByteData &byteData)
{
    cout<<"--------------------------------------"<<endl;
    int size = byteData.getLength();
    cout<<"Byte data length: "<<size<<endl;
    const uint8_t *mData =byteData.getByteData();
    int i=0;
    while (i<size)
    {
        int data = mData[i];
        cout<<"  "<< data;
        i++;
    }
    cout<<"\n----------------------------------------"<<endl;
}

class EZMQSubCallback: public EZMQSUBCallback
{
   public:
        void onMessageCB(const EZMQMessage &event)
        {
            cout<<"App: Event received: onMessageCB "<<endl;
            if(EZMQ_CONTENT_TYPE_PROTOBUF == event.getContentType())
            {
                cout<<"Content-Type: EZMQ_CONTENT_TYPE_PROTOBUF"<<endl;
                const Event *protoEvent =  dynamic_cast<const Event*>(&event);
                printEvent(*protoEvent);
            }
            else if(EZMQ_CONTENT_TYPE_BYTEDATA== event.getContentType())
            {
                cout<<"Content-Type: EZMQ_CONTENT_TYPE_BYTEDATA"<<endl;
                const EZMQByteData *byteData =  dynamic_cast<const EZMQByteData*>(&event);
                printByteData(*byteData);
            }
        }

        void onMessageCB(const std::string &topic, const EZMQMessage &event)
        {
            cout<<"App: Event received : onMessageCB"<<endl;
            cout<<"Topic: "<<topic<<endl;
            if(EZMQ_CONTENT_TYPE_PROTOBUF == event.getContentType())
            {
                cout<<"Content-Type: EZMQ_CONTENT_TYPE_PROTOBUF"<<endl;
                const Event *protoEvent =  dynamic_cast<const Event*>(&event);
                printEvent(*protoEvent);
            }
            else if(EZMQ_CONTENT_TYPE_BYTEDATA== event.getContentType())
            {
                cout<<"Content-Type: EZMQ_CONTENT_TYPE_BYTEDATA"<<endl;
                const EZMQByteData *byteData =  dynamic_cast<const EZMQByteData*>(&event);
                printByteData(*byteData);
            }
        }
};

void sigint(int /*signal*/)
{
    if (gIsStarted)
    {
        std::unique_lock<std::mutex> lock(gMutex);
        gCV.notify_all();
    }
    else
    {
        exit (EXIT_FAILURE);
    }
}

void printError()
{
    cout<<"\nRe-run the application as shown in below examples: "<<endl;
    cout<<"\n  (1) For subscribing without topic: "<<endl;
    cout<<"     ./subscriber -ip 0.0.0.0 -port 5562"<<endl;
    cout<<"     ./subscriber -ip localhost -port 5562"<<endl;
    cout<<"\n  (2) For subscribing with topic: "<<endl;
    cout<<"     ./subscriber -ip 0.0.0.0 -port 5562 -t topic1"<<endl;
    cout<<"     ./subscriber -ip localhost -port 5562 -t topic1"<<endl;
}

int main(int argc, char* argv[])
{
    std::string ip;
    int port = 5562;
    std::string topic="";
    int secured = 0;
    EZMQErrorCode result = EZMQ_ERROR;
    gIsStarted = false;

    // get ip and port from command line arguments
    if(argc != 5 && argc!=7 && argc != 9)
    {
        printError();
        return -1;
    }
    int n = 1;
    while (n < argc)
    {
        if (0 == strcmp(argv[n],"-ip"))
        {
            ip = argv[n + 1];
            cout<<"Given Ip: " <<ip<<endl;
            n = n + 2;
        }
        else if (0 == strcmp(argv[n],"-port"))
        {
            port = atoi(argv[n + 1]);
            cout<<"Given Port: " << port <<endl;
            n = n + 2;
        }
        else if (0 == strcmp(argv[n],"-t"))
        {
            topic = argv[n + 1];
            cout<<"Topic is : " << topic<<endl;
            n = n + 2;
        }
        else if (0 == strcmp(argv[n],"-secured"))
        {
            secured = atoi(argv[n + 1]);
            cout<<"Secured : " << secured<<endl;
            n = n + 2;
        }
        else
        {
            printError();
        }
    }

    //this handler is added to check stop API
    signal(SIGINT, sigint);

    //Initialize EZMQ stack
    EZMQAPI *obj = EZMQAPI::getInstance();
    result = obj->initialize();
    std::cout<<"\nInitialize API [result]: "<<result<<endl;
    if(result != EZMQ_OK)
    {
        return -1;
    }

    //Create EZMQ Subscriber
    EZMQSubCallback *callback = new EZMQSubCallback();
    gSubscriber =  new(std::nothrow) EZMQSubscriber(ip, port, callback);
    if(NULL == gSubscriber)
    {
        std::cout<<"gSubscriber creation failed !!"<<endl;
        abort();
    }
    std::cout<<"Subscriber created !!"<<endl;

    // set the server and client keys
    if(1 == secured)
    {
        try
        {
            gSubscriber->setServerPublicKey(gServerPublicKey);
            gSubscriber->setClientKeys(gClientSecretKey, gClientPublicKey);
        }
        catch(EZMQException &e)
        {
            cout<<"Exception caught in set keys: "<<e.what() << endl;
            return -1;
        }
    }

    //Start EZMQ Subscriber
    result = gSubscriber->start();
    cout<<"Subscriber start [Result] : "<<result<<endl;
    if(result != EZMQ_OK)
    {
        return -1;
    }
    gIsStarted = true;
    //subscribe for events
    if (topic.empty())
    {
        result = gSubscriber->subscribe();
    }
    else
    {
        result = gSubscriber->subscribe(topic);
    }

    if (result != EZMQ_OK)
    {
        cout<<"subscribe API: error occured"<<endl;
        return -1;
    }

    cout<<"Suscribed to publisher.. -- Waiting for Events --"<<endl;
    std::unique_lock<std::mutex> lock(gMutex);
    gCV.wait(lock);

    if(nullptr!=gSubscriber)
    {
        cout<<"callig stop API: "<<endl;
        result = gSubscriber->stop();
        cout<<"stop API: [Result]: "<<result<<std::endl;
    }
return 0;
}

