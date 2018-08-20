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
#include "EZMQSubscriber.h"
#include "EZMQLogger.h"
#include "EZMQByteData.h"
#include "EZMQException.h"

#define TCP_PREFIX "tcp://"
#define INPROC_PREFIX "inproc://shutdown-"
#define TOPIC_PATTERN "[a-zA-Z0-9-_./]+"
#define CONTENT_TYPE_OFFSET 5
#define VERSION_OFFSET 2
#define VERSION_MASK 0x07
#define KEY_LENGTH 40
#define TAG "EZMQSubscriber"

#ifdef __GNUC__
#define EZMQ_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

namespace ezmq
{
    EZMQSubscriber::EZMQSubscriber(const std::string &ip, const int &port, EZMQSubCB subCallback, EZMQSubTopicCB topicCallback):
        mIp(ip), mPort(port), mSubCallback(subCallback), mSubTopicCallback(topicCallback)
    {
        mContext = EZMQAPI::getInstance()->getContext();
        if(nullptr == mContext)
        {
            EZMQ_LOG(ERROR, TAG, "[Constructor] Context is null");
        }
        mShutdownServer = nullptr;
        mShutdownClient = nullptr;
        mSubscriber = nullptr;
        isReceiverStarted = false;
        mCallback= NULL;
    }

    EZMQSubscriber::EZMQSubscriber(const std::string &ip, const int &port, EZMQSUBCallback *callback):
        mIp(ip), mPort(port), mCallback(callback)
    {
        mContext = EZMQAPI::getInstance()->getContext();
        if(nullptr == mContext)
        {
            EZMQ_LOG(ERROR, TAG, "[Constructor] Context is null");
        }
        mShutdownServer = nullptr;
        mShutdownClient = nullptr;
        mSubscriber = nullptr;
        isReceiverStarted = false;
    }

    EZMQSubscriber::~EZMQSubscriber()
    {
        stop();
    }

    void EZMQSubscriber::parseSocketData()
    {
        zmq::message_t zFrame1;
        zmq::message_t zFrame2;
        zmq::message_t zFrame3;
        void *data;
        size_t size;
        EZMQMessage msg;
        ezmq::Event event;
        EZMQByteData byteData{NULL,0};
        std::string topic;
        int version;
        int contentType;
        bool isTopic = false;
        unsigned char *ezmqHeader ;

        std::lock_guard<std::recursive_mutex> lock(mSubLock);
        if(mSubscriber)
        {
            try
            {
                mSubscriber->recv(&zFrame1);
                if(zFrame1.more())
                {
                    mSubscriber->recv(&zFrame2);
                    if(zFrame2.more())
                    {
                        isTopic = true;
                        mSubscriber->recv(&zFrame3);
                    }
                }
            }
            catch (std::exception &e)
            {
                EZMQ_LOG_V(ERROR, TAG, "[receive] caught exception: %s", e.what());
                isReceiverStarted = false;
                return;
            }
        }
        else
        {
            return;
        }

        if(false == isTopic)
        {
            //header
            ezmqHeader = (unsigned char *)zFrame1.data();

            //data
            data = zFrame2.data();
            size = zFrame2.size();
        }
        else
        {
            //header
            ezmqHeader = (unsigned char *)zFrame2.data();

            //topic
            std::string topicStr(static_cast<char*>(zFrame1.data()), zFrame1.size());
            topic = topicStr;
            if (topic.at(topic.length()-1) == '/')
            {
                topic.erase(topic.length()-1);
            }

            //data
            data = zFrame3.data();
            size = zFrame3.size();
        }

        contentType = ezmqHeader[0] >> CONTENT_TYPE_OFFSET;
        version = (ezmqHeader[0] >> VERSION_OFFSET) & VERSION_MASK;

        //data
        if(EZMQ_CONTENT_TYPE_PROTOBUF == contentType)
        {
            event.mContentType = EZMQ_CONTENT_TYPE_PROTOBUF;
            event.mVersion = version;
            std::string msgStr(static_cast<char*>(data), size);
            event.ParseFromString(msgStr);
            //call application callback
            if(false == isTopic)
            {
                if(NULL == mCallback)
                {
                    mSubCallback(event);
                    return;
                }
                mCallback->onMessageCB(event);
            }
            else
            {
                if(NULL == mCallback)
                {
                    mSubTopicCallback(topic, event);
                    return;
                }
                mCallback->onMessageCB(topic, event);
            }
        }
        else if(EZMQ_CONTENT_TYPE_BYTEDATA == contentType)
        {
            byteData.mContentType = EZMQ_CONTENT_TYPE_BYTEDATA;
            byteData.mVersion = version;
            byteData.mData = (uint8_t *)data;
            byteData.mDataLength = size;
            //call application callback
            if(false == isTopic)
            {
                if(NULL == mCallback)
                {
                    mSubCallback(byteData);
                    return;
                }
                mCallback->onMessageCB(byteData);
            }
            else
            {
                if(NULL == mCallback)
                {
                    mSubTopicCallback(topic, byteData);
                    return;
                }
                mCallback->onMessageCB(topic, byteData);
            }
        }
        else
        {
            EZMQ_LOG_V(ERROR, TAG, "[receive] Not a supported type: %d", contentType);
        }
    }

    void EZMQSubscriber::receive()
    {
        while(isReceiverStarted)
        {
            if (nullptr == mSubscriber || mPollItems.empty())
            {
                EZMQ_LOG(ERROR, TAG, "[receive] subscriber or poller is null");
                return;
            }

            zmq::poll(mPollItems);
            if (mPollItems[1].revents & ZMQ_POLLIN)
            {
                parseSocketData();
            }
            else if(mPollItems[0].revents & ZMQ_POLLIN)
            {
                EZMQ_LOG(DEBUG, TAG, "[receive] Shut down request");
                break;
            }
        }
    }

    EZMQErrorCode EZMQSubscriber::setClientKeys(const std::string& clientPrivateKey,
        const std::string& clientPublicKey)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
#ifdef SECURITY_ENABLED
        if (clientPrivateKey.length() != KEY_LENGTH || clientPublicKey.length() != KEY_LENGTH)
        {
            EZMQ_LOG(ERROR, TAG, "Invalid length of public/private key");
            return EZMQ_ERROR;
        }
        mClientSecretKey = clientPrivateKey;
        mClientPublicKey= clientPublicKey;
#else
        UNUSED(clientPrivateKey);
        UNUSED(clientPublicKey);
        throw EZMQException("Security is not enabled");
#endif // SECURITY_ENABLED
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQSubscriber::setServerPublicKey(const std::string& key)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
#ifdef SECURITY_ENABLED
        if (key.length() != KEY_LENGTH)
        {
            EZMQ_LOG(ERROR, TAG, "Invalid key length");
            return EZMQ_ERROR;
        }
        mServerPublicKey = key;
#else
        UNUSED(key);
        throw EZMQException("Security is not enabled");
#endif // SECURITY_ENABLED
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQSubscriber::start()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        VERIFY_NON_NULL(mContext)
        try
        {
            std::lock_guard<std::recursive_mutex> lock(mSubLock);
            std::string address = getInProcUniqueAddress();
            // Shutdown server sockets
            if (!mShutdownServer)
            {
                mShutdownServer =  new zmq::socket_t(*mContext, ZMQ_PAIR);
                ALLOC_ASSERT(mShutdownServer)
                mShutdownServer->bind(address);
            }

            // Shutdown client sockets
            if (!mShutdownClient)
            {
                mShutdownClient = new zmq::socket_t(*mContext, ZMQ_PAIR);
                ALLOC_ASSERT(mShutdownClient)
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
                ALLOC_ASSERT(mSubscriber)
#ifdef SECURITY_ENABLED
                //Set sever public key
                if (mServerPublicKey.length() == KEY_LENGTH)
                {
                    mSubscriber->setsockopt(ZMQ_CURVE_SERVERKEY, mServerPublicKey.c_str(),
                        mServerPublicKey.size());
                }
                //Set Client public key
                if (mClientPublicKey.length() == KEY_LENGTH)
                {
                    mSubscriber->setsockopt(ZMQ_CURVE_PUBLICKEY, mClientPublicKey.c_str(),
                        mClientPublicKey.size());
                }
                //Set Client private/secret key
                if (mClientSecretKey.length() == KEY_LENGTH)
                {
                    mSubscriber->setsockopt(ZMQ_CURVE_SECRETKEY, mClientSecretKey.c_str(),
                        mClientSecretKey.size());
                }
                // clear the keys
                clearKeys();
#endif // SECURITY_ENABLED

                std::string address = getSocketAddress(mIp, mPort);
                mSubscriber->connect(address);
                EZMQ_LOG_V(DEBUG, TAG, "Starting subscriber [Address]: %s", address.c_str());

                 // Register sockets to poller
                zmq_pollitem_t subscriberPoller;
                subscriberPoller.socket = *mSubscriber;
                subscriberPoller.events = ZMQ_POLLIN;
                mPollItems.push_back(subscriberPoller);
            }
        }
        catch (std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "[start] caught exception: %s", e.what());
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
        std::string topic = "";
        return subscribeInternal(topic);
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

    EZMQErrorCode EZMQSubscriber::subscribeInternal(std::string &topic)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        VERIFY_NON_NULL(mContext)
        std::lock_guard<std::recursive_mutex> lock(mSubLock);
        try
        {
            VERIFY_NON_NULL(mSubscriber)
            mSubscriber->setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
        }
        catch (std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "[Subscribe] caught exception: %s", e.what());
            return EZMQ_ERROR;
        }
        EZMQ_LOG(DEBUG, TAG, "subscribed for events");
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQSubscriber::subscribe(const std::list<std::string> &topics)
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

    EZMQErrorCode EZMQSubscriber::subscribe(const std::string &ip, const int &port, std::string topic)
    {
        EZMQ_SCOPE_LOGGER(TAG, "subscribe [Topic]");
        if(ip.empty() || port < 0 )
        {
            return EZMQ_ERROR;
        }
        //Validate Topic
        topic = sanitizeTopic(topic);
        if(topic.empty())
        {
            return EZMQ_INVALID_TOPIC;
        }
        EZMQ_LOG_V(DEBUG, TAG, "IP: %s Port: %d Topic: %s",  ip.c_str(),  port, topic.c_str());
        return subscribeInternal(ip, port, topic);
    }

     EZMQErrorCode EZMQSubscriber::subscribeInternal(const std::string &ip, const int &port, std::string &topic)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        VERIFY_NON_NULL(mContext)
        std::lock_guard<std::recursive_mutex> lock(mSubLock);
        try
        {
            VERIFY_NON_NULL(mSubscriber)

#ifdef SECURITY_ENABLED
            //Set sever public key
            if (mServerPublicKey.length() == KEY_LENGTH)
            {
                mSubscriber->setsockopt(ZMQ_CURVE_SERVERKEY, mServerPublicKey.c_str(),
                    mServerPublicKey.size());
            }
            //Set Client public key
            if (mClientPublicKey.length() == KEY_LENGTH)
            {
                mSubscriber->setsockopt(ZMQ_CURVE_PUBLICKEY, mClientPublicKey.c_str(),
                    mClientPublicKey.size());
            }
            //Set Client private/secret key
            if (mClientSecretKey.length() == KEY_LENGTH)
            {
                mSubscriber->setsockopt(ZMQ_CURVE_SECRETKEY, mClientSecretKey.c_str(),
                    mClientSecretKey.size());
            }
            // clear the keys
            clearKeys();
#endif // SECURITY_ENABLED

            mSubscriber->connect(getSocketAddress(ip, port));
            mSubscriber->setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
        }
        catch (std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "[Subscribe] caught exception: %s", e.what());
            return EZMQ_ERROR;
        }
        EZMQ_LOG(DEBUG, TAG, "subscribed for events");
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQSubscriber::unSubscribe()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        std::string topic = "";
        return unSubscribeInternal(topic);
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

    EZMQErrorCode EZMQSubscriber::unSubscribeInternal(std::string &topic)
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        VERIFY_NON_NULL(mContext)
        std::lock_guard<std::recursive_mutex> lock(mSubLock);
        try
        {
            VERIFY_NON_NULL(mSubscriber)
            mSubscriber->setsockopt(ZMQ_UNSUBSCRIBE ,  topic.c_str(), topic.size());
        }
        catch (std::exception e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
            return EZMQ_ERROR;
        }
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQSubscriber::unSubscribe(const std::list<std::string> &topics)
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
        VERIFY_NON_NULL(mContext)
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
            if(isReceiverStarted)
            {
                mThread.join();
            }

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

            // clear the keys
            clearKeys();

            // close subscriber socket
            if (mSubscriber)
            {
                mSubscriber->close();
                delete mSubscriber;
                mSubscriber = nullptr;
            }
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
        }

        //clear the poll item vector
        if(isReceiverStarted)
        {
            mPollItems.clear();
        }

        //Reset receiver flag
        isReceiverStarted = false;

        EZMQ_LOG(DEBUG, TAG, "Subscriber stopped");
        return EZMQ_OK;
    }

    std::string& EZMQSubscriber::getIp()
    {
        return mIp;
    }

    int EZMQSubscriber::getPort()
    {
        return mPort;
    }

    std::string EZMQSubscriber::getSocketAddress(const std::string &ip, const int &port)
    {
        try
        {
            return TCP_PREFIX + ip+ ":"  + std::to_string(port);
        }
        catch(std::exception &e)
        {
            EZMQ_LOG_V(ERROR, TAG, "caught exception: %s", e.what());
        }
        return "";
    }

    std::string EZMQSubscriber::getInProcUniqueAddress()
    {
        std::string address = INPROC_PREFIX + std::to_string(std::rand());
        EZMQ_LOG_V(DEBUG, TAG, "The in proc address is:: %s", address.c_str());
        return address;
    }

    std::string EZMQSubscriber::sanitizeTopic(std::string &topic)
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

    void EZMQSubscriber::clearKeys()
    {
        // clear the keys
        if(!mServerPublicKey.empty())
        {
            const char *serverPublickey = mServerPublicKey.data();
            memset((char *)serverPublickey, '\0', mServerPublicKey.length()-1);
            mServerPublicKey = "";
        }

        if(!mClientPublicKey.empty())
        {
            const char *clientPublicKey = mClientPublicKey.data();
            memset((char *)clientPublicKey, '\0', mClientPublicKey.length()-1);
            mClientPublicKey = "";
        }

        if(!mClientSecretKey.empty())
        {
            const char *clientSecretKey = mClientSecretKey.data();
            memset((char *)clientSecretKey, '\0', mClientSecretKey.length()-1);
            mClientSecretKey = "";
        }
    }
}

