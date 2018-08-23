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

#include "EZMQException.h"

#include "UnitTestHelper.h"

using namespace ezmq;

class EZMQExceptionTest: public TestWithMock
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

TEST_F(EZMQExceptionTest, constructorTest)
{
    EZMQException *exceptionObj = new EZMQException();
    ASSERT_NE(nullptr, exceptionObj);
    delete exceptionObj;
}

TEST_F(EZMQExceptionTest, constructorTest1)
{
    std::string exceptionMsg = "Test";
    EZMQException *exceptionObj = new EZMQException(exceptionMsg);
    ASSERT_NE(nullptr, exceptionObj);
    delete exceptionObj;
}

TEST_F(EZMQExceptionTest, whatTest)
{
    std::string exceptionMsg = "Test";
    EZMQException *exceptionObj = new EZMQException(exceptionMsg);
    ASSERT_NE(nullptr, exceptionObj);
    std::string msg = exceptionObj->what();
    EXPECT_EQ(exceptionMsg, msg);
    delete exceptionObj;
}

