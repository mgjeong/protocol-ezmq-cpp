
#include "UnitTestHelper.h"
#include "EZMQAPI.h"
#include "EZMQLogger.h"
#include "EZMQSubscriber.h"

#include <iostream>

#define TAG "EZMQ_PUB_TEST"

using namespace ezmq;

void subCB(ezmq::Event /*event*/)
{
    EZMQ_LOG(DEBUG, TAG, "Event received");
}

void subTopicCB(std::string topic, ezmq::Event /*event*/)
{
    EZMQ_LOG(DEBUG, TAG, "Event received");
    EZMQ_LOG_V(DEBUG, TAG, "Topic: %s", topic.c_str());
    UNUSED(topic);
}

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
        mSubscriber = new EZMQSubscriber(mIp, mPort,  subCB,  subTopicCB);
        TestWithMock::SetUp();
    }

    void TearDown()
    {
        mSubscriber->stop();
        apiInstance->terminate();
        TestWithMock::TearDown();
    }

    EZMQAPI *apiInstance;
    EZMQSubscriber *mSubscriber;
    std::string  mTopic;
    std::string mIp;
    int mPort;
};

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

TEST_F(EZMQSubscriberTest, subscribeTopic)
{
    EXPECT_EQ(EZMQ_OK, mSubscriber->start());
    EXPECT_EQ(EZMQ_OK, mSubscriber->subscribe(mTopic));
}

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