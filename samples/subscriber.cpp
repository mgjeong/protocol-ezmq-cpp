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

#include "EZMQAPI.h"
#include "EZMQSubscriber.h"
#include "EZMQErrorCodes.h"
#include "Event.pb.h"

#include <iostream>
#include <unistd.h>
#include <signal.h>

#include <condition_variable>
#include <memory>

using namespace std;
using namespace ezmq;

EZMQSubscriber *subscriber = nullptr ;

bool isStarted;
std::mutex m_mutex;
std::condition_variable m_cv;

void printEvent(ezmq::Event event)
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
void subCB(ezmq::Event event)
{
    cout<<"App: Event received "<<endl;
    printEvent(event);
}

void subTopicCB(std::string topic, ezmq::Event event)
{
    cout<<"App: Event received "<<endl;
    cout<<"Topic: "<<topic<<endl;
    printEvent(event);
}

void sigint(int /*signal*/)
{
    if (isStarted)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.notify_all();
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
    cout<<"     ./subscriber -ip 107.108.81.116 -port 5562"<<endl;
    cout<<"     ./subscriber -ip localhost -port 5562"<<endl;
    cout<<"\n  (2) For subscribing with topic: "<<endl;
    cout<<"     ./subscriber -ip 107.108.81.116 -port 5562 -t topic1"<<endl;
    cout<<"     ./subscriber -ip localhost -port 5562 -t topic1"<<endl;
}

int main(int argc, char* argv[])
{
    std::string ip;
    int port = 5562;
    std::string topic="";
    EZMQErrorCode result = EZMQ_ERROR;
    isStarted = false;

    // get ip and port from command line arguments
    if(argc != 5 && argc != 7)
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
    subscriber =  new EZMQSubscriber(ip, port,  subCB,  subTopicCB);
    if(nullptr ==subscriber )
    {
        cout<<"Subscriber is null "<<endl;
        return -1;
    }
    std::cout<<"Subscriber created !!"<<endl;

    //Start EZMQ Subscriber
    result = subscriber->start();
    cout<<"Subscriber start [Result] : "<<result<<endl;
    if(result != EZMQ_OK)
    {
        return -1;
    }
    isStarted = true;
    //subscribe for events
    if (topic.empty())
    {
        result = subscriber->subscribe();
    }
    else
    {
        result = subscriber->subscribe(topic);
    }

    if (result != EZMQ_OK)
    {
        cout<<"subscribe API: error occured"<<endl;
        return -1;
    }

    cout<<"Suscribed to publisher.. -- Waiting for Events --"<<endl;
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock);

    if(nullptr!=subscriber)
    {
        cout<<"callig stop API: "<<endl;
        result = subscriber->stop();
        cout<<"stop API: [Result]: "<<result<<std::endl;
    }
return 0;
}
