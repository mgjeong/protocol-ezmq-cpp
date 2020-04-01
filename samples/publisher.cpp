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

#ifdef __linux__
#include <unistd.h>
#endif
#if defined(_WIN32)
#include <signal.h>
#endif

#include "EZMQAPI.h"
#include "EZMQPublisher.h"
#include "EZMQMessage.h"
#include "EZMQByteData.h"
#include "EZMQErrorCodes.h"
#include "EZMQException.h"
#include "Event.pb.h"

#if defined(_WIN32)
#define sleep(x) Sleep(x)
#define DELAY 2000
#else
#define DELAY 2
#endif

using namespace std;
using namespace ezmq;

EZMQPublisher *gPublisher  = nullptr;
// put server secret key
std::string gServerSecretKey = "";

void startCB(EZMQErrorCode /*code*/)
{
    cout<<"Start callback ";
}

void stopCB(EZMQErrorCode /*code*/)
{
    cout<<"stop callback ";
}

void errorCB(EZMQErrorCode /*code*/)
{
    cout<<"Error callback ";
}

ezmq::Event getProtoBufEvent()
{
    ezmq::Event event;
    event.set_device("device");
    event.set_created(10);
    event.set_modified(20);
    event.set_id("id");
    event.set_pushed(10);
    event.set_origin(20);

    ezmq::Reading *reading1 = event.add_reading();
    reading1->set_name("reading1");
    reading1->set_value("10");
    reading1->set_created(25);
    reading1->set_device("device");
    reading1->set_modified(20);
    reading1->set_id("id1");
    reading1->set_origin(25);
    reading1->set_pushed(1);

    ezmq::Reading *readin2  = event.add_reading();
    readin2->set_name("reading2");
    readin2->set_value("20");
    readin2->set_created(30);
    readin2->set_device("device");
    readin2->set_modified(20);
    readin2->set_id("id2");
    readin2->set_origin(25);
    readin2->set_pushed(1);


    return event;
}

void printError()
{
    cout<<"\nRe-run the application as shown in below examples: "<<endl;
    cout<<"\n  (1) For publishing without topic: "<<endl;
    cout<<"     ./publisher -port 5562"<<endl;
    cout<<"\n  (2) For publishing without topic [Secured]: "<<endl;
    cout<<"     ./publisher -port 5562 -secured 1"<<endl;
    cout<<"\n  (3) For publishing with topic: "<<endl;
    cout<<"      ./publisher -port 5562 -t topic1"<<endl;
    cout<<"\n  (4) For publishing with topic [Secured]: "<<endl;
    cout<<"      ./publisher -port 5562 -t topic1 -secured 1"<<endl;
}

void sigint(int /*signal*/)
{
    if(gPublisher)
    {
        cout<<"-- Destroying publisher-- "<<endl;
        delete gPublisher;
        gPublisher = NULL;
    }
}

int main(int argc, char* argv[])
{
    int port = 5562;
    std::string topic="";
    int secured = 0;
    EZMQErrorCode result = EZMQ_ERROR;

    // get port from command line arguments
    if(argc != 3 && argc != 5 && argc != 7)
    {
        printError();
        return -1;
    }
    int n = 1;
    while (n < argc)
    {
        if (0 == strcmp(argv[n],"-port"))
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

    //Create EZMQ Publisher
    gPublisher = new(std::nothrow) EZMQPublisher(port, startCB,  stopCB,  errorCB);
    if(NULL == gPublisher)
    {
        std::cout<<"Publisher creation failed !!"<<endl;
        abort();
    }
    std::cout<<"Publisher created !!"<<endl;

    // set the server key
    if(1 == secured)
    {
        try
        {
            result = gPublisher->setServerPrivateKey(gServerSecretKey);
        }
        catch(EZMQException &e)
       {
            cout<<"Exception caught in setServerPrivateKey: "<<e.what() << endl;
            return -1;
       }
    }

    //Start EZMQ Publisher
    result = gPublisher->start();
    cout<<"Publisher start [Result] : "<<result<<endl;
    if(result != EZMQ_OK)
    {
        return -1;
    }

    // get Proto EZMQ event
    ezmq::Event event = getProtoBufEvent();

    // This delay is added to prevent ZeroMQ first packet drop during
    // initial connection of publisher and subscriber.
    sleep(1);

    cout<<"--------- Will Publish 15 events at interval of 2 seconds --------- "<<endl;
    int i = 1;
    while(i <= 15)
    {
        //This check is required while used ctrl+c for program termination
        if(!gPublisher)
        {
            return -1;
        }
        if (topic.empty())
        {
            result = gPublisher->publish(event);
        }
        else
        {
            result = gPublisher->publish(topic, event);
        }
        if(result != EZMQ_OK)
        {
            cout<<"publish API: error occured"<<endl;
            return 0;
        }
        cout<<"Event "<<i <<" Published" <<endl;
        sleep(DELAY);
        i++;
    }

    if(gPublisher)
    {
        cout<<"-- Destroying publisher-- "<<endl;
        delete gPublisher;
    }
return 0;
}

