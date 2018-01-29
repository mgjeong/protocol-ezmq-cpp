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

#include <regex>

#include "EZMQAPI.h"
#include "EZMQPublisher.h"
#include "EZMQLogger.h"
#include "zmq_addon.hpp"

#define PUB_TCP_PREFIX "tcp://*:"
#define TOPIC_PATTERN "[a-zA-Z0-9-_./]+"
#define TAG "EZMQPublisher"

#ifdef __GNUC__
#define EZMQ_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

namespace ezmq
{
    EZMQPublisher::EZMQPublisher(int port, EZMQStartCB startCB, EZMQStopCB stopCB, EZMQErrorCB errorCB):
        mPort(port), mStartCallback(startCB), mStopCallback(stopCB), mErrorCallback(errorCB)
    {
        mContext = EZMQAPI::getInstance()->getContext();
        if(nullptr == mContext)
        {
             EZMQ_LOG(ERROR, TAG, "Context is null");
        }
        mPublisher = nullptr;
    }

    EZMQPublisher::~EZMQPublisher()
    {
        stop();
        if(mPublisher)
        {
           delete mPublisher;
           mPublisher = nullptr;
        }
    }

    EZMQErrorCode EZMQPublisher::start()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        try
        {
            std::lock_guard<std::recursive_mutex> lock(mPubLock);
            if(nullptr == mPublisher)
            {
                VERIFY_NON_NULL(mContext)
                mPublisher = new(std::nothrow) zmq::socket_t(*mContext, ZMQ_PUB);
                ALLOC_ASSERT(mPublisher)
                mPublisher->bind(getSocketAddress());
            }
        }
        catch (std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "[start] caught exception %s", e.what());
            delete mPublisher;
            mPublisher = nullptr;
            return EZMQ_ERROR;
        }
        EZMQ_LOG_V(DEBUG, TAG, "Publisher started [address]: %s", getSocketAddress().c_str());
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQPublisher::publish(ezmq::Event event)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        std::string eventStr;
        bool result  = event.SerializeToString(&eventStr);
        if (false == result)
        {
            return EZMQ_ERROR;
        }

        // create a zmq message from the serialized string
        zmq::message_t zmqMsg (eventStr.size());
        memcpy ((void *) zmqMsg.data (), eventStr.c_str(), eventStr.size());
        std::lock_guard<std::recursive_mutex> lock(mPubLock);
        try
        {
            VERIFY_NON_NULL(mPublisher)
            result = mPublisher->send(zmqMsg);
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "[publish] caught exception %s", e.what());
            return EZMQ_ERROR;
        }
        if (false == result)
        {
            EZMQ_LOG(ERROR, TAG, "Published without topic failed");
            return EZMQ_ERROR;
        }
        EZMQ_LOG(DEBUG, TAG, "Published without topic");
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQPublisher::publish( std::string topic, ezmq::Event event)
    {
        EZMQ_SCOPE_LOGGER(TAG, "publish [Topic]");
        //Validate Topic
        topic = sanitizeTopic(topic);
        if(topic.empty())
        {
            return EZMQ_INVALID_TOPIC;
        }

        //prepare a multi part message with topic
        zmq::multipart_t zmqMultipart(topic);

        // create a zmq message from the serialized string
        std::string eventStr;
        bool result  = event.SerializeToString(&eventStr);
         if (false == result)
        {
            return EZMQ_ERROR;
        }
        zmqMultipart.add(zmq::message_t(eventStr.c_str(), eventStr.size()));

        //publish on the socket
        std::lock_guard<std::recursive_mutex> lock(mPubLock);
        try
        {
            VERIFY_NON_NULL(mPublisher)
            result = zmqMultipart.send(*mPublisher);
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
            return EZMQ_ERROR;
        }
        if (false == result)
        {
            EZMQ_LOG_V(ERROR, TAG, "Publish failed");
            return EZMQ_ERROR;
        }
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQPublisher::publish( std::list<std::string> topics, ezmq::Event event)
    {
        EZMQ_SCOPE_LOGGER(TAG, "publish [List Topic]");
        if(!topics.size())
        {
            return EZMQ_INVALID_TOPIC;
        }

        EZMQErrorCode result = EZMQ_OK;
        for (auto topic : topics)
        {
            result = publish(topic, event);
            if (result != EZMQ_OK)
            {
                return result;
            }
        }
        return result;
    }

    EZMQErrorCode EZMQPublisher::stop()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        EZMQErrorCode result = EZMQ_ERROR;
        std::lock_guard<std::recursive_mutex> lock(mPubLock);

        // Sync close
        result = syncClose();
        delete mPublisher;
        mPublisher = nullptr;
        EZMQ_LOG(DEBUG, TAG, "publisher stopped");
        return result;
    }

    int EZMQPublisher::getPort()
    {
        return mPort;
    }

    std::string EZMQPublisher::getSocketAddress()
    {
        return PUB_TCP_PREFIX + std::to_string(mPort);
    }

    std::string EZMQPublisher::sanitizeTopic(std::string topic)
    {
        if(topic.empty())
        {
            return topic;
        }

    //Regex support is supported from  gcc-4.9 and higher
#if defined(EZMQ_GCC_VERSION) && EZMQ_GCC_VERSION >= 40900
    std::regex pattern(TOPIC_PATTERN);
    if(!std::regex_match(topic, pattern))
    {
        return "";
    }
#endif

        if (topic.at(topic.length()-1) != '/')
        {
            topic = topic + "/";
        }
        return topic;
    }

    std::string getMonitorAddress()
    {
        std::string MONITOR_PREFIX = "inproc://monitor-";
        std::string address = MONITOR_PREFIX + std::to_string(std::rand());
        EZMQ_LOG_V(DEBUG, TAG, "monitor address is: %s", address.c_str());
        return address;
    }

    EZMQErrorCode EZMQPublisher::syncClose()
    {
        zmq::monitor_t monitor;
        VERIFY_NON_NULL(mPublisher)
        monitor.init(*mPublisher, getMonitorAddress(), ZMQ_EVENT_CLOSED);
        try
        {
            VERIFY_NON_NULL(mPublisher)
            mPublisher->close();
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception while closing publisher: %s", e.what());
            return EZMQ_ERROR;
        }

        if(true == monitor.check_event(1000))
        {
            EZMQ_LOG(DEBUG, TAG, "Received ZMQ_EVENT_CLOSED Event");
        }
        else
        {
            EZMQ_LOG(ERROR, TAG, "No ZMQ_EVENT_CLOSED event");
        }
        return EZMQ_OK;
    }
}

