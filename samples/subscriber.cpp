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
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.notify_all();
}

int main()
{
    std::string ip = "localhost";
    int port = 5562;
    int choice =0;
    std::string topic="";

    //this handler is added to check stop API
    signal(SIGINT, sigint);

    //Initialize EZMQ stack
    EZMQAPI *obj = EZMQAPI::getInstance();
    std::cout<<"Initialize API [result]: "<<obj->initialize()<<endl;

    cout<<"Host: ";
    cin>>ip;
    cout<<"Port: ";
    cin>>port;

    cout<<"Enter 1 for General Event testing"<<endl;
    cout<<"Enter 2 for Topic Based delivery"<<endl;
    cout<<"ctrl+c to terminate the program"<<endl<<endl;
    cin>>choice;

    //Create EZMQ Subscriber
    switch (choice)
    {
        case 1:
            subscriber =  new EZMQSubscriber(ip, port,  subCB,  subTopicCB);
            break;
        case 2:
            subscriber =  new EZMQSubscriber(ip, port,  subCB,  subTopicCB);
            cout<<"Enter the topic: ";
            cin>>topic;
            cout<<"Topic is: "<<topic<<endl;
            break;
        default:
            cout<<"Invalid choice..[Re-run application]"<<endl;
            return -1;
    }

    if(nullptr ==subscriber )
    {
        cout<<"Subscriber is null "<<endl;
        return 0;
    }
    std::cout<<"Subscriber created !!"<<endl;

    //Start EZMQ Subscriber
    EZMQErrorCode result = subscriber->start();
    cout<<"Subscriber start [Result] : "<<result<<endl;

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
