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

/**
  * @file   EZMQSubscriber.h
  *
  * @brief This file provides APIs for subscriber: start, subscribe, stop.
  */

#ifndef EZMQ_SUBSCRIBER_H
#define EZMQ_SUBSCRIBER_H

#include <list>
#include <thread>
#include <mutex>

//Protobuf header file
#include "Event.pb.h"

//ZeroMQ header file
#include "zmq.hpp"

#include "EZMQErrorCodes.h"
#include "EZMQMessage.h"

namespace ezmq
{
    /**
    * Callbacks to get all the subscribed events.
    */
    typedef std::function<void(const EZMQMessage &event)> EZMQSubCB;

    /**
    * Callbacks to get all the subscribed events for a specific topic.
    */
    typedef std::function<void(std::string topic, const EZMQMessage &event)> EZMQSubTopicCB;

    /**
    * @class  EZMQSubscriber
    * @brief   This class Contains the APIs related to start, stop, subscribe APIs
    *               of EZMQ Subscriber.
    */
    class EZMQSubscriber
    {
        public:

            /**
            *  Construtor of EZMQSubscriber.
            *
            * @param ip - ip to be used for subscriber socket.
            * @param port - Port to be used for subscriber socket.
            * @param subCallback- Subscriber callback to receive events.
            * @param topicCallback - Subscriber callback to receive events for a particular topic.
            */
            EZMQSubscriber(std::string ip, int port, EZMQSubCB subCallback, EZMQSubTopicCB topicCallback);

            /**
            *  Construtor of EZMQSubscriber.
            *
            * @param serviceName -Subscribe to given serviceName.
            * @param subCallback- Subscriber callback to receive events.
            * @param topicCallback - Subscriber callback to receive events for a particular topic.
            */
            EZMQSubscriber(std::string serviceName, EZMQSubCB subCallback, EZMQSubTopicCB topicCallback);

            /**
            * Destructor of EZMQPublisher.
            */
            ~EZMQSubscriber();

            /**
            * Starts SUB  instance.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode start();

            /**
            *  Subscribe for event/messages.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode subscribe();

            /**
            * Subscribe for event/messages on a particular topic.
            *
            * @param topic - Topic to be subscribed.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            * @note (1) Topic name should be as path format. For example:
            *       home/livingroom/ (2) Topic name can have letters [a-z, A-z],
            *       numerics [0-9] and special characters _ - . and /
            */
            EZMQErrorCode subscribe( std::string topic);

            /**
            * Subscribe for event/messages on given list of topics. On any of the topic
            * in list, if it failed to subscribe events it will return
            * EZMQ_ERROR/EZMQ_INVALID_TOPIC.
            *
            * @param topic - List of topics to be subscribed
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            *  @note (1) Topic name should be as path format. For example:
            *       home/livingroom/ (2) Topic name can have letters [a-z, A-z],
            *       numerics [0-9] and special characters _ - . and /
            */
            EZMQErrorCode subscribe(std::list<std::string> topics);

            /**
            * Un-subscribe all the events from publisher.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode unSubscribe() ;

            /**
            * Un-subscribe specific topic events.
            *
            * @param topic - topic to be unsubscribed.
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            * @note (1) Topic name should be as path format. For example:
            *       home/livingroom/ (2) Topic name can have letters [a-z, A-z],
            *       numerics [0-9] and special characters _ - . and /
            */
            EZMQErrorCode unSubscribe(std::string topic);

            /**
            * Un-subscribe event/messages on given list of topics. On any of the topic
            * in list, if it failed to unsubscribe events it will return
            * EZMQ_ERROR/EZMQ_INVALID_TOPIC.
            *
            * @param topic - List of topics to be unsubscribed
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            *  @note (1) Topic name should be as path format. For example:
            *       home/livingroom/ (2) Topic name can have letters [a-z, A-z],
            *       numerics [0-9] and special characters _ - . and /
            */
            EZMQErrorCode unSubscribe(std::list<std::string> topics);

            /**
            * Stops SUB instance.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode stop();

            /**
            * Get the IP address.
            *
            * @return IP address as String.
            */
            std::string& getIp();

            /**
            * Get the port of the subscriber.
            *
            * @return port number as integer.
            */
            int getPort();

            /**
            * Get the service name.
            *
            * @return Service name.
            */
            std::string getServiceName();

        private:
            std::string mServiceName;
            std::string mIp;
            int mPort;

            //Receiver Thread
            std::thread mThread;
            bool isReceiverStarted;

            //EZMQ callbacks
            EZMQSubCB mSubCallback;
            EZMQSubTopicCB mSubTopicCallback;

            // ZMQ Subscriber socket
            zmq::socket_t * mSubscriber;
            std::shared_ptr<zmq::context_t> mContext;

            // ZMQ shut-down sockets
            zmq::socket_t *mShutdownServer;
            zmq::socket_t *mShutdownClient;

            // ZMQ poller
            std::vector<zmq::pollitem_t> mPollItems;

            //Mutex
            std::recursive_mutex mSubLock;

            EZMQErrorCode subscribeInternal(std::string topic);
            EZMQErrorCode unSubscribeInternal(std::string topic);
            std::string getSocketAddress();
            std::string getInProcUniqueAddress();
            void receive();
            void parseSocketData();
            std::string  sanitizeTopic(std::string topic);
    };
}
#endif //EZMQ_SUBSCRIBER_H

