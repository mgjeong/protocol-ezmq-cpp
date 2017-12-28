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

