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
#include <iostream>

#include "EZMQByteData.h"
#include "EZMQLogger.h"

#define TAG "EZMQByteData"

namespace ezmq
{
    EZMQByteData::EZMQByteData(const uint8_t *data,  size_t length):mData(data), mDataLength(length)
    {
        mContentType = EZMQ_CONTENT_TYPE_BYTEDATA;
    }

    EZMQByteData::~EZMQByteData()
    {
    }

    size_t EZMQByteData::getLength() const
    {
        return mDataLength;
    }

    const uint8_t * EZMQByteData::getByteData() const
    {
        return mData;
    }

    EZMQErrorCode EZMQByteData::setByteData(const uint8_t * data, size_t dataLength)
    {
        VERIFY_NON_NULL(data)
        if(dataLength == 0)
        {
            return EZMQ_ERROR;
        }
        mData = data;
        mDataLength = dataLength;
        return EZMQ_OK;
    }
}

