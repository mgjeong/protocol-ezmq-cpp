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

#if defined(_WIN32)
#define ZMQ_STATIC
#pragma once
#include "zmq_addon.hpp"
#undef ZMQ_STATIC
#else
#include "zmq_addon.hpp"
#endif

#include "EZMQAPI.h"
#include "EZMQPublisher.h"
#include "EZMQLogger.h"
#include "EZMQByteData.h"

#define PUB_TCP_PREFIX "tcp://*:"
#define TOPIC_PATTERN "[a-zA-Z0-9-_./]+"
#define EZMQ_VERSION 1
#define EZMQ_HEADER 0x00
#define CONTENT_TYPE_OFFSET 5
#define VERSION_OFFSET 2
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
        if(mPublisher)
        {
            stop();
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

    EZMQErrorCode EZMQPublisher::publishInternal( std::string topic, const EZMQMessage &event)
    {
        // Form EZMQ header
        unsigned char ezmqHeader = EZMQ_HEADER;
        unsigned char version = EZMQ_VERSION ;
        version = version << VERSION_OFFSET;
        ezmqHeader =  ezmqHeader | version;
        unsigned char contentType;
        if(EZMQ_CONTENT_TYPE_PROTOBUF == event.getContentType())
        {
            contentType = EZMQ_CONTENT_TYPE_PROTOBUF;
        }
        else if(EZMQ_CONTENT_TYPE_BYTEDATA == event.getContentType())
        {
            contentType = EZMQ_CONTENT_TYPE_BYTEDATA;
        }
        else
        {
            EZMQ_LOG(ERROR, TAG, "Not a supported content-type");
            return EZMQ_INVALID_CONTENT_TYPE;
        }
        contentType = contentType << CONTENT_TYPE_OFFSET;
        ezmqHeader =  ezmqHeader | contentType;

        zmq::multipart_t zmqMultipart;
        try
        {
            // EZMQ Topic [ZMQMessage]
            if(!(topic.empty()))
            {
                zmqMultipart.addstr(topic);
            }

            //EZMQ header [ZMQMessage]
            zmqMultipart.add(zmq::message_t((void *)(&ezmqHeader), sizeof(ezmqHeader)));

            //EZMQ Data [ZMQMessage]
            if(EZMQ_CONTENT_TYPE_PROTOBUF == event.getContentType())
            {
                const Event *protoEvent =  dynamic_cast<const Event*>(&event);
                if(NULL == protoEvent)
                {
                    EZMQ_LOG(ERROR, TAG, "[protoEvent] dynamic_cast failed");
                    return EZMQ_ERROR;
                }
                std::string eventStr;
                bool result = protoEvent->SerializeToString(&eventStr);
                if (false == result)
                {
                    return EZMQ_ERROR;
                }
                zmqMultipart.add(zmq::message_t(eventStr.c_str(), eventStr.size()));
            }
            else if(EZMQ_CONTENT_TYPE_BYTEDATA == event.getContentType())
            {
                const EZMQByteData *byteData =  dynamic_cast<const EZMQByteData*>(&event);
                if(NULL == byteData)
                {
                    EZMQ_LOG(ERROR, TAG, "[ByteData] dynamic_cast failed");
                    return EZMQ_ERROR;
                }
                if(NULL == byteData->getByteData())
                {
                    EZMQ_LOG(ERROR, TAG, "[ByteData] Byte Data is NULL");
                    return EZMQ_ERROR;
                }
                zmqMultipart.add(zmq::message_t(byteData->getByteData(), byteData->getLength()));
            }
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "[publish] caught exception %s", e.what());
            return EZMQ_ERROR;
        }

        //send data [ZMQMessage] on socket
        std::lock_guard<std::recursive_mutex> lock(mPubLock);
        bool result = false;
        try
        {
            VERIFY_NON_NULL(mPublisher)
            result = zmqMultipart.send(*mPublisher);
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "[publish] caught exception %s", e.what());
            return EZMQ_ERROR;
        }
        if (false == result)
        {
            EZMQ_LOG(ERROR, TAG, "Publish failed");
            return EZMQ_ERROR;
        }
        EZMQ_LOG(DEBUG, TAG, "Published data");
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQPublisher::publish(const EZMQMessage &event)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        return publishInternal("", event);
    }

    EZMQErrorCode EZMQPublisher::publish( std::string topic, const EZMQMessage &event)
    {
        EZMQ_SCOPE_LOGGER(TAG, "publish [Topic]");
        //Validate Topic
        topic = sanitizeTopic(topic);
        if(topic.empty())
        {
            return EZMQ_INVALID_TOPIC;
        }
        return publishInternal(topic, event);
    }

    EZMQErrorCode EZMQPublisher::publish( std::list<std::string> topics, const EZMQMessage &event)
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
        try
        {
            return PUB_TCP_PREFIX + std::to_string(mPort);
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
        }
        return "";
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
        try
        {
            if (topic.at(topic.length()-1) != '/')
            {
                topic = topic + "/";
            }
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "Allocation failed: %s", e.what());
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
        try
        {
            monitor.init(*mPublisher, getMonitorAddress(), ZMQ_EVENT_CLOSED);
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

