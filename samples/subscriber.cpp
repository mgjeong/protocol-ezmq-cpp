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

void sigint(int signal)
{
    if(nullptr!=subscriber)
    {
        cout<<"callig stop API: "<<endl;
        EZMQErrorCode result = subscriber->stop();
        cout<<"stop API: [Result]: "<<result<<std::endl;
		//m_cv.notify_all();
    }
    exit(0);
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
    std::unique_lock<std::mutex> lk(m_mutex);
    m_cv.wait(lk);
	
return 0;
}
