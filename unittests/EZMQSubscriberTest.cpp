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
#include "EZMQLogger.h"
#include "EZMQSubscriber.h"
#include "UnitTestHelper.h"

#define TAG "EZMQ_PUB_TEST"
#define SERVER_PUBLIC_KEY "tXJx&1^QE2g7WCXbF.$$TVP.wCtxwNhR8?iLi&S<"
#define CLIENT_SECRET_KEY "ZB1@RS6Kv^zucova$kH(!o>tZCQ.<!Q)6-0aWFmW"
#define CLIENT_PUBLIC_KEY "-QW?Ved(f:<::3d5tJ$[4Er&]6#9yr=vha/caBc("

using namespace ezmq;

void subCB(const EZMQMessage &/*event*/)
{
    EZMQ_LOG(DEBUG, TAG, "Event received");
}

void subTopicCB(std::string topic, const EZMQMessage &/*event*/)
{
    EZMQ_LOG(DEBUG, TAG, "Event received");
    EZMQ_LOG_V(DEBUG, TAG, "Topic: %s", topic.c_str());
    UNUSED(topic);
}

class EZMQSubCallback: public EZMQSUBCallback
{
   public:
        void onMessageCB(const EZMQMessage &/*event*/)
        {
            EZMQ_LOG(DEBUG, TAG, "Event received");
        }

        void onMessageCB(const std::string &topic, const EZMQMessage &/*event*/)
        {
            EZMQ_LOG(DEBUG, TAG, "Event received");
            EZMQ_LOG_V(DEBUG, TAG, "Topic: %s", topic.c_str());
            UNUSED(topic);
        }
};

class EZMQSubscriberTest: public TestWithMock
{
protected:
    void SetUp()
    {
        mTopic = "topic";
        mIp = "localhost";
        mPort = 5562;
        apiInstance = EZMQAPI::getInstance();
        ASSERT_NE(nullptr, apiInstance);
        EXPECT_EQ(EZMQ_OK, apiInstance->initialize());
        mSubscriber = new(std::nothrow) EZMQSubscriber(mIp, mPort,  subCB,  subTopicCB);
        ALLOC_ASSERT(mSubscriber)
        TestWithMock::SetUp();
    }

    void TearDown()
    {
        mSubscriber->stop();
        delete mSubscriber;
        apiInstance->terminate();
        TestWithMock::TearDown();
    }

    EZMQAPI *apiInstance;
    EZMQSubscriber *mSubscriber;
    std::string  mTopic;
    std::string mIp;
    int mPort;
};

TEST_F(EZMQSubscriberTest, constructor)
{
    EZMQSubCallback *callback = new EZMQSubCallback();
    EZMQSubscriber *subscriber = new(std::nothrow)  EZMQSubscriber(mIp, mPort, callback);
    ALLOC_ASSERT(subscriber)
    ASSERT_NE(nullptr, subscriber);
}


TEST_F(EZMQSubscriberTest, start)
{
   EXPECT_EQ(EZMQ_OK, mSubscriber->start());
}

TEST_F(EZMQSubscriberTest, startstop)
{
    for( int i =1; i<=10; i++)
    {
        EXPECT_EQ(EZMQ_OK, mSubscriber->start());
        EXPECT_EQ(EZMQ_OK, mSubscriber->stop());
    }
}

TEST_F(EZMQSubscriberTest, subscribe)
{
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe());
}

#ifdef SECURITY_ENABLED
TEST_F(EZMQSubscriberTest, subscribeSecured)
{
    EXPECT_EQ(EZMQ_OK, mSubscriber->setServerPublicKey(SERVER_PUBLIC_KEY));
    EXPECT_EQ(EZMQ_OK, mSubscriber->setClientKeys(CLIENT_SECRET_KEY, CLIENT_PUBLIC_KEY));
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe());
}

TEST_F(EZMQSubscriberTest, subscribeNegative)
{
    EXPECT_EQ(EZMQ_ERROR, mSubscriber->setServerPublicKey(""));
    EXPECT_EQ(EZMQ_ERROR, mSubscriber->setClientKeys("", ""));
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe());
}
#endif // SECURITY_ENABLED

TEST_F(EZMQSubscriberTest, subscribeTopic)
{
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(mTopic));
}

TEST_F(EZMQSubscriberTest, subscribeTopicIPPort)
{
    std::string topic = "topic";
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(mIp, mPort, topic));
    EXPECT_EQ(EZMQ_ERROR, mSubscriber->subscribe("", mPort, topic));
    EXPECT_EQ(EZMQ_ERROR, mSubscriber->subscribe(mIp, -1, topic));
    topic = "";
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mSubscriber->subscribe(mIp, mPort, topic));
}

#ifdef SECURITY_ENABLED
TEST_F(EZMQSubscriberTest, subscribeTopicIPPortSecured)
{
    std::string topic = "topic";
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->setServerPublicKey(SERVER_PUBLIC_KEY));
    EXPECT_EQ(EZMQ_OK, mSubscriber->setClientKeys(CLIENT_SECRET_KEY, CLIENT_PUBLIC_KEY));
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(mIp, mPort, topic));
}
#endif // SECURITY_ENABLED

TEST_F(EZMQSubscriberTest, subscribeTopicList)
{
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(mTopic));

    std::list<std::string> topicList;
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mSubscriber->subscribe(topicList));

    topicList.push_back("topic1");
    topicList.push_back("");
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mSubscriber->subscribe(topicList));

    topicList.clear();
    topicList.push_back("topic1");
    topicList.push_back("topic2");
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(topicList));
}

TEST_F(EZMQSubscriberTest, subscribeTopics)
{
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(mTopic));

      // Empty topic test
    std::string testingTopic = "";
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mSubscriber->subscribe(testingTopic));

    // Numeric test
    testingTopic = "123";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));

    // Alpha-Numeric test
    testingTopic = "1a2b3";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));

    // Alphabet forward slash test
    testingTopic = "topic/";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));

    // Alphabet-Numeric, forward slash test
    testingTopic = "topic/13/4jtjos/";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));

    // Alphabet-Numeric, forward slash test
    testingTopic = "123a/1this3/4jtjos";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));

    // Topic contain forward slash at last
    testingTopic = "topic/122/livingroom/";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));

    // Topic contain -
    testingTopic = "topic/122/livingroom/-";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));

    // Topic contain _
    testingTopic = "topic/122/livingroom_";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));

    // Topic contain .
    testingTopic = "topic/122.livingroom.";
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(testingTopic));
}

TEST_F(EZMQSubscriberTest, unSubscribe)
{
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe());
    EXPECT_EQ(EZMQ_OK, mSubscriber->unSubscribe());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(mTopic));
    EXPECT_EQ(EZMQ_OK, mSubscriber->unSubscribe(mTopic));
}


TEST_F(EZMQSubscriberTest, unSubscribeTopicList)
{
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());

    std::list<std::string> topicList;
    topicList.push_back("topic1");
    topicList.push_back("");
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mSubscriber->subscribe(topicList));
    EXPECT_EQ(EZMQ_INVALID_TOPIC, mSubscriber->unSubscribe(topicList));

    topicList.clear();
    topicList.push_back("topic1");
    topicList.push_back("topic2");
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(topicList));
    EXPECT_EQ(EZMQ_OK, mSubscriber->unSubscribe(topicList));
}

TEST_F(EZMQSubscriberTest, getIp)
{
    EXPECT_EQ(mIp, mSubscriber->getIp());
}

TEST_F(EZMQSubscriberTest, getPort)
{
    EXPECT_EQ(mPort, mSubscriber->getPort());
}

