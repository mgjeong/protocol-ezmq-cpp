#include "EZMQAPI.h"
#include "EZMQPublisher.h"
#include "EZMQErrorCodes.h"
#include "Event.pb.h"

#include <iostream>
#include <unistd.h>

using namespace std;
using namespace ezmq;

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

int main()
{
    int port = 5562;
    int choice =0;
    std::string topic="";
    EZMQPublisher *publisher  = NULL;

     //Initialize EZMQ stack
    EZMQAPI *obj = EZMQAPI::getInstance();
    std::cout<<"Initialize API [result]: "<<obj->initialize()<<endl;

    cout<<"Enter 1 for General Event testing"<<endl;
    cout<<"Enter 2 for Topic Based delivery"<<endl;
    cin>>choice;

    //Create EZMQ Publisher
    switch (choice)
    {
        case 1:
            publisher = new EZMQPublisher(port, startCB,  stopCB,  errorCB);
            break;
        case 2:
            publisher = new EZMQPublisher(port, startCB,  stopCB,  errorCB);
            cout<<"Enter the topic: ";
            cin>>topic;
            cout<<"Topic is: "<<topic<<endl;
            break;
        default:
            cout<<"Invalid choice..[Re-run application]"<<endl;
            return -1;
    }

    std::cout<<"Publisher created !!"<<endl;

    //Start EZMQ Publisher
    EZMQErrorCode result = publisher->start();
    cout<<"Publisher start [Result] : "<<result<<endl;

    // get Proto EZMQ event
    ezmq::Event event = getProtoBufEvent();

    cout<<"--------- Will Publish 15 events at interval of 2 seconds --------- "<<endl;
    int i = 1;
    while(i <= 15)
    {
        if (topic.empty())
        {
            result = publisher->publish(event);
        } else
        {
            result = publisher->publish(topic, event);
        }
        if(result != EZMQ_OK)
        {
            cout<<"publish API: error occured"<<endl;
            return 0;
        }
        cout<<"Event "<<i <<" Published" <<endl;
        sleep(2);
        i++;
    }
return 0;
}
