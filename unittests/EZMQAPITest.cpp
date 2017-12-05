
#include "UnitTestHelper.h"
#include "EZMQAPI.h"

#include <iostream>

using namespace ezmq;

class EZMQAPITest: public TestWithMock
{
protected:
    void SetUp()
    {
        TestWithMock::SetUp();
    }

    void TearDown()
    {
        TestWithMock::TearDown();
    }
};

TEST_F(EZMQAPITest, getEZMQInstance)
{
    EZMQAPI *obj = EZMQAPI::getInstance();
    EXPECT_EQ(EZMQ_Constructed, obj->getStatus());
    ASSERT_NE(nullptr, obj);
}

TEST_F(EZMQAPITest, initialize)
{
    EZMQAPI *obj = EZMQAPI::getInstance();
    EXPECT_EQ(EZMQ_OK, obj->initialize());
}

TEST_F(EZMQAPITest, terminate)
{
    EZMQAPI *obj = EZMQAPI::getInstance();
    EXPECT_EQ(EZMQ_OK, obj->initialize());
    EXPECT_EQ(EZMQ_OK, obj->terminate());
}

TEST_F(EZMQAPITest, getStatus)
{
    EZMQAPI *obj = EZMQAPI::getInstance();
    obj->initialize();
    EXPECT_EQ(EZMQ_Initialized, obj->getStatus());
    obj->terminate();
    EXPECT_EQ(EZMQ_Terminated, obj->getStatus());
}

