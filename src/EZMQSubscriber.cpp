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
#include "EZMQLogger.h"

#include <iostream>
#include <string>
#include <sstream>
#include <regex>

#define TCP_PREFIX "tcp://"
#define INPROC_PREFIX "inproc://shutdown-"
#define TOPIC_PATTERN "[a-zA-Z0-9-_./]+"
#define TAG "EZMQSubscriber"

#ifdef __GNUC__
#define EZMQ_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

namespace ezmq
{
    EZMQSubscriber::EZMQSubscriber(std::string ip, int port, EZMQSubCB subCallback, EZMQSubTopicCB topicCallback):
        mIp(ip), mPort(port), mSubCallback(subCallback), mSubTopicCallback(topicCallback)
    {
        mContext = EZMQAPI::getInstance()->getContext();
        if(nullptr == mContext)
        {
            EZMQ_LOG(ERROR, TAG, "Context is null");
        }
        mShutdownServer = nullptr;
        mShutdownClient = nullptr;
        mSubscriber = nullptr;
        isReceiverStarted = false;
    }

    EZMQSubscriber::EZMQSubscriber(std::string serviceName, EZMQSubCB subCallback, EZMQSubTopicCB topicCallback):
        mServiceName(serviceName), mSubCallback(subCallback), mSubTopicCallback(topicCallback)
    {
        //TOOD
    }

    EZMQSubscriber::~EZMQSubscriber()
    {
        stop();
    }

    void EZMQSubscriber::receive()
    {
        zmq::message_t zMsg;
        zmq::message_t zTopic;
        ezmq::Event event;
        std::string topic;
        bool more = false;
        while(isReceiverStarted)
        {
            if (nullptr == mSubscriber || mPollItems.empty())
            {
                EZMQ_LOG(ERROR, TAG, "subscriber or poller is null");
                return;
            }

            zmq::poll(mPollItems);
            if (mPollItems[1].revents & ZMQ_POLLIN)
            {
                std::lock_guard<std::recursive_mutex> lock(mSubLock);
                if(mSubscriber)
                {
                    try
                    {
                        mSubscriber->recv(&zMsg);
                        more = false;
                        if(zMsg.more())
                        {
                            more = true;
                            std::string topicStr(static_cast<char*>(zMsg.data()), zMsg.size());
                            topic = topicStr;
                            mSubscriber->recv(&zMsg);
                        }
                    }
                    catch (std::exception e)
                    {
                        EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
                        isReceiverStarted = false;
                        break;
                    }
                }

                //convert data to ezmq event
                std::string msgStr(static_cast<char*>(zMsg.data()), zMsg.size());
                event.ParseFromString(msgStr);

                //call  application callback
                if(more)
                {
                    mSubTopicCallback(topic, event);
                }
                else
                {
                    mSubCallback(event);
                }
            }
            else if(mPollItems[0].revents & ZMQ_POLLIN)
            {
                EZMQ_LOG(DEBUG, TAG, "Received the shut down request");
                break;
            }
        }
    }

    EZMQErrorCode EZMQSubscriber::start()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        if (nullptr == mContext)
        {
            EZMQ_LOG(ERROR, TAG, "Context is null");
            return EZMQ_ERROR;
        }
        try
        {
            std::lock_guard<std::recursive_mutex> lock(mSubLock);
            std::string address = getInProcUniqueAddress();
            // Shutdown server sockets
            if (!mShutdownServer)
            {
                mShutdownServer =  new zmq::socket_t(*mContext, ZMQ_PAIR);
                mShutdownServer->bind(address);
            }

            // Shutdown client sockets
            if (!mShutdownClient)
            {
                mShutdownClient = new zmq::socket_t(*mContext, ZMQ_PAIR);
                mShutdownClient->connect(address);
                zmq_pollitem_t shutDownPoller;
                shutDownPoller.socket = *mShutdownClient;
                shutDownPoller.events = ZMQ_POLLIN;
                mPollItems.push_back(shutDownPoller);
            }

            // Subscriber socket
            if(!mSubscriber)
            {
                mSubscriber = new zmq::socket_t(*mContext, ZMQ_SUB);
                mSubscriber->connect(getSocketAddress());
                EZMQ_LOG_V(DEBUG, TAG, "Starting subscriber [Address]: %s", getSocketAddress().c_str());

                 // Register sockets to poller
                zmq_pollitem_t subscriberPoller;
                subscriberPoller.socket = *mSubscriber;
                subscriberPoller.events = ZMQ_POLLIN;
                mPollItems.push_back(subscriberPoller);
            }
        }
        catch (std::exception e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
            stop();
            return EZMQ_ERROR;
        }

        std::lock_guard<std::recursive_mutex> lock(mSubLock);
        //receiver Thread
        if(!isReceiverStarted)
        {
            isReceiverStarted = true;
            mThread = std::thread(&EZMQSubscriber::receive, this);
        }
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQSubscriber::subscribe()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        return subscribeInternal("");
    }

    EZMQErrorCode EZMQSubscriber::subscribe(std::string topic)
    {
        EZMQ_SCOPE_LOGGER(TAG, "subscribe [Topic]");
        //Validate Topic
        topic = sanitizeTopic(topic);
        if(topic.empty())
        {
            return EZMQ_INVALID_TOPIC;
        }
        EZMQ_LOG_V(DEBUG, TAG, "Topic: %s", topic.c_str());
        return subscribeInternal(topic);
    }

    EZMQErrorCode EZMQSubscriber::subscribeInternal(std::string topic)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        if(!mSubscriber)
        {
            EZMQ_LOG(ERROR, TAG, "subscriber is null");
            return EZMQ_ERROR;
        }

        std::lock_guard<std::recursive_mutex> lock(mSubLock);
        try
        {
            if(mSubscriber)
            {
                mSubscriber->setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
            }
            else
            {
                return EZMQ_ERROR;
            }
        }
        catch (std::exception e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
            return EZMQ_ERROR;
        }
        EZMQ_LOG(DEBUG, TAG, "subscribed for events");
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQSubscriber::subscribe(std::list<std::string> topics)
    {
        EZMQ_SCOPE_LOGGER(TAG, "subscribe [Topic List]");
        if(!topics.size())
        {
            EZMQ_LOG(ERROR, TAG, "Topic list is empty");
            return EZMQ_INVALID_TOPIC;
        }

        EZMQErrorCode result = EZMQ_OK;
        for (auto topic : topics)
        {
            result = subscribe(topic);
            if (result != EZMQ_OK)
            {
                return result;
            }
        }
        return result;
    }

    EZMQErrorCode EZMQSubscriber::unSubscribe()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        return unSubscribeInternal("");
    }

    EZMQErrorCode EZMQSubscriber::unSubscribe(std::string topic)
    {
        EZMQ_SCOPE_LOGGER(TAG, "unSubscribe [Topic]");
        //Validate Topic
        topic = sanitizeTopic(topic);
        if(topic.empty())
        {
            return EZMQ_INVALID_TOPIC;
        }
        EZMQ_LOG_V(DEBUG, TAG, "Topic: %s", topic.c_str());
        return unSubscribeInternal(topic);
    }

    EZMQErrorCode EZMQSubscriber::unSubscribeInternal(std::string topic)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        if(!mSubscriber)
        {
            EZMQ_LOG(ERROR, TAG, "subscriber is null");
            return EZMQ_ERROR;
        }

        std::lock_guard<std::recursive_mutex> lock(mSubLock);
        try
        {
            if(mSubscriber)
            {
                mSubscriber->setsockopt(ZMQ_UNSUBSCRIBE ,  topic.c_str(), topic.size());
            }
            else
            {
                return EZMQ_ERROR;
            }
        }
        catch (std::exception e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
            return EZMQ_ERROR;
        }
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQSubscriber::unSubscribe(std::list<std::string> topics)
    {
        EZMQ_SCOPE_LOGGER(TAG, "unSubscribe [Topic list]");
        if(!topics.size())
        {
            return EZMQ_INVALID_TOPIC;
        }

        EZMQErrorCode result = EZMQ_OK;
        for (auto topic : topics)
        {
            result = unSubscribe(topic);
            if (result != EZMQ_OK)
            {
                return result;
            }
        }
        return result;
    }

    EZMQErrorCode EZMQSubscriber::stop()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        std::lock_guard<std::recursive_mutex> lock(mSubLock);
        try
        {
            // Send a shutdown message to receiver thread
            if (mShutdownServer)
            {
                std::string msg = "shutdown";
                zmq::message_t zMsg (msg.size());
                memcpy ((void *) zMsg.data (), msg.c_str(), msg.size());
                bool result =  mShutdownServer->send(zMsg);
                UNUSED(result);
                EZMQ_LOG_V(DEBUG, TAG, "Shut down request sent[Result]: %d", result);
            }

            //wait for shutdown msg to be sent to receiver thread
            mThread.join();

            // close shut down client socket
            if (mShutdownClient)
            {
                mShutdownClient->close();
                delete mShutdownClient;
                mShutdownClient = nullptr;
            }

            // close shut down server socket
            if (mShutdownServer)
            {
                mShutdownServer->close();
                delete mShutdownServer;
                mShutdownServer = nullptr;
            }

            // close subscriber socket
            if (mSubscriber)
            {
                mSubscriber->close();
                delete mSubscriber;
                mSubscriber = nullptr;
            }
        }
        catch(std::exception e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
        }

        //clear the poll item vector
        mPollItems.clear();

        //Reset receiver flag
        isReceiverStarted = false;

        EZMQ_LOG(DEBUG, TAG, "Subscriber stopped");
        return EZMQ_OK;
    }

        std::string EZMQSubscriber::getIp()
        {
            return mIp;
        }

        int EZMQSubscriber::getPort()
        {
            return mPort;
        }

        std::string  EZMQSubscriber::getServiceName()
        {
            return mServiceName;
        }

    std::string EZMQSubscriber::getSocketAddress()
    {
        return TCP_PREFIX + mIp + ":"  + std::to_string(mPort);
    }

    std::string EZMQSubscriber::getInProcUniqueAddress()
    {
        std::string address = INPROC_PREFIX + std::to_string(std::rand());
        EZMQ_LOG_V(DEBUG, TAG, "The in proc address is:: %s", address.c_str());
        return address;
    }

    std::string EZMQSubscriber::sanitizeTopic(std::string topic)
    {
        if(topic.empty())
        {
            return "";
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
}
