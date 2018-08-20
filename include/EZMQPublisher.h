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
  * @file   EZMQPublisher.h
  *
  * @brief This file provides APIs for publisher: start, subscribe, stop.
  */

#ifndef EZMQ_PUBLISHER_H
#define EZMQ_PUBLISHER_H

#include <list>
#include <mutex>

//Protobuf header file
#include "Event.pb.h"

//ZeroMQ header file
#include "zmq.hpp"

#include "EZMQMessage.h"
#include "EZMQErrorCodes.h"

namespace ezmq
{
    /**
    * Callbacks to get error codes for start/stop of EZMQ publisher.
    * Note: As of now not being used.
    */
    typedef std::function<void(EZMQErrorCode code)> EZMQStartCB;
    typedef std::function<void(EZMQErrorCode code)> EZMQStopCB;
    typedef std::function<void(EZMQErrorCode code)> EZMQErrorCB;

    /**
     * Interface to receive callback from EZMQ.
     * Note: As of now not being used.
     */
    class EZMQPUBCallback
    {
        public:
            /**
             * Invoked on start of PUB instance.
             *
             * @param errorCode - EZMQ_OK on success, otherwise appropriate error code.
             */
            virtual void onStartCB(EZMQErrorCode /*errorCode*/) {};

            /**
             * Invoked on stop of SUB instance.
             *
             * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
             */
            virtual void onStopCB(EZMQErrorCode /*errorCode*/) {};

            /**
             * Invoked on error of PUB instance.
             *
             * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
             */
            virtual void onErrorCB(EZMQErrorCode /*errorCode*/) {};
    };

    /**
    * @class  EZMQPublisher
    * @brief   This class Contains the APIs related to start, stop, publish APIs
    *               of EZMQ Publisher.
    */
    class EZMQPublisher
    {
        public:
            /**
            * Construtor of EZMQPublisher.
            *
            * @param port - Port to be used for publisher socket.
            * @param startCB- Start callback.
            * @param stopCB - Stop Callback.
            * @param errorCB - Error Callback.
            */
            EZMQPublisher(const int &port, EZMQStartCB startCB, EZMQStopCB stopCB, EZMQErrorCB errorCB);

            /**
            * Construtor of EZMQPublisher.
            *
            * @param port - Port to be used for publisher socket.
            * @param callback - Callback interface object.
            */
            EZMQPublisher(const int &port, EZMQPUBCallback *callback);

            /**
            * Destructor of EZMQPublisher.
            */
            ~EZMQPublisher();

            /**
            * Set the server private/secret key.
            *
            * @param key - Server private/Secret key.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            * @throws EZMQException
            *
            * @note
            * (1) Key should be 40-character string encoded in the Z85 encoding format <br>
            * (2) This API should be called before start() API.
            */
            EZMQErrorCode setServerPrivateKey(const std::string& key);

            /**
            * Starts PUB instance.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode start();

            /**
            * Publish events on the socket for subscribers.
            *
            * @param event - event to be published.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode publish(const EZMQMessage &event);

            /**
            * Publish events on a specific topic on socket for subscribers.
            *
            * @param topic - Topic on which event needs to be published.
            * @param event - event to be published.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            * @note
            * (1) Topic name should be as path format. For example: home/livingroom/<br>
            * (2) Topic name can have letters [a-z, A-z], numerics [0-9] and special characters _ - . and /
            */
            EZMQErrorCode publish(std::string topic, const EZMQMessage &event);

            /**
            * Publish an events on list of topics on socket for subscribers. On any of
            * the topic in list, if it failed to publish event it will return
            * EZMQ_ERROR/EZMQ_INVALID_TOPIC.
            *
            * @param topic - List of Topics on which event needs to be published.
            * @param event - event to be published.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            * @note
            * (1) Topic name should be as path format. For example: home/livingroom/<br>
            * (2) Topic name can have letters [a-z, A-z], numerics [0-9] and special characters _ - . and /
            */
            EZMQErrorCode publish(const std::list<std::string> &topics, const EZMQMessage &event);

            /**
            * Stops PUB instance.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode stop();

            /**
            * Get the port of the publisher.
            *
            * @return port number as integer.
            */
            int getPort();

        private:
            int mPort;
            std::string mServerSecretKey;

            //callbacks
            EZMQStartCB mStartCallback;
            EZMQStopCB mStopCallback;
            EZMQErrorCB mErrorCallback;
            EZMQPUBCallback *mPubCallback;

            //ZMQ socket
            zmq::socket_t *mPublisher;
            std::shared_ptr<zmq::context_t> mContext;

            //Mutex
            std::recursive_mutex mPubLock;

            EZMQErrorCode publishInternal(std::string &topic, const EZMQMessage &event);
            std::string getSocketAddress();
            std::string  sanitizeTopic(std::string &topic);
            EZMQErrorCode syncClose();
    };
}
#endif //EZMQ_PUBLISHER_H

