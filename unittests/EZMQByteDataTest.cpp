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

#include "EZMQByteData.h"
#include "UnitTestHelper.h"

using namespace ezmq;

class EZMQByteDataTest: public TestWithMock
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

TEST_F(EZMQByteDataTest, constructByteData)
{
    char byteArray[] = { 0x40, 0x05, 0x10, 0x11, 0x12 };
    EZMQByteData byteData((uint8_t *) byteArray, sizeof(byteArray));
    EXPECT_EQ(EZMQ_CONTENT_TYPE_BYTEDATA, byteData.getContentType());
}

TEST_F(EZMQByteDataTest, setByteData)
{
    char byteArray[] = { 0x40, 0x05, 0x10, 0x11, 0x12 };
    EZMQByteData byteData((uint8_t *) byteArray, sizeof(byteArray));
    char byteArray1[] = { 0x44, 0x15, 0x16, 0x18, 0x19 };
    int original = byteArray1[0];
    EXPECT_EQ(EZMQ_OK, byteData.setByteData((uint8_t *) byteArray1, sizeof(byteArray1)));
    const uint8_t *getData = byteData.getByteData();
    int updated = getData[0];
    EXPECT_EQ(original, updated);
}

TEST_F(EZMQByteDataTest, setByteDataNegative)
{
    char byteArray[] = { 0x40, 0x05, 0x10, 0x11, 0x12 };
    EZMQByteData *byteData = new EZMQByteData((uint8_t *) byteArray, sizeof(byteArray));
    if(byteData)
    {
        EXPECT_EQ(EZMQ_ERROR, byteData->setByteData(NULL, sizeof(byteArray)));
        EXPECT_EQ(EZMQ_ERROR, byteData->setByteData((uint8_t *) byteArray, 0));
        delete byteData;
    }
}

TEST_F(EZMQByteDataTest, constructByteDataPointer)
{
    char byteArray[] = { 0x40, 0x05, 0x10, 0x11, 0x12 };
    EZMQByteData *byteData = new EZMQByteData((uint8_t *) byteArray, sizeof(byteArray));
    if(byteData)
    {
        EXPECT_EQ(EZMQ_CONTENT_TYPE_BYTEDATA, byteData->getContentType());
        delete byteData;
    }
}

TEST_F(EZMQByteDataTest, constructByteDataNull)
{
    EZMQByteData *byteData = new EZMQByteData(NULL,0);
    if(byteData)
    {
        EXPECT_EQ(EZMQ_CONTENT_TYPE_BYTEDATA, byteData->getContentType());
        delete byteData;
    }
}

TEST_F(EZMQByteDataTest, setContentTypel)
{
    EZMQByteData *byteData = new EZMQByteData(NULL,0);
    if(byteData)
    {
        byteData->setContentType(EZMQ_CONTENT_TYPE_BYTEDATA);
        EXPECT_EQ(EZMQ_CONTENT_TYPE_BYTEDATA, byteData->getContentType());
        delete byteData;
    }
}

TEST_F(EZMQByteDataTest, setInvalidContentType)
{
    EZMQByteData *byteData = new EZMQByteData(NULL,0);
    if(byteData)
    {
        EXPECT_EQ(EZMQ_INVALID_CONTENT_TYPE,
            byteData->setContentType(EZMQ_CONTENT_TYPE_JSON));
        delete byteData;
    }
}

