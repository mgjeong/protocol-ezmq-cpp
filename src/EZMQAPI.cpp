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

#define TAG "EZMQAPI"

namespace ezmq
{
    EZMQAPI *EZMQAPI::getInstance()
    {
        static EZMQAPI instance;
        return &instance;
    }

    EZMQErrorCode EZMQAPI::initialize()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        if(nullptr == mContext)
        {
            try
            {
                mContext = std::make_shared<zmq::context_t>(1);
            }
            catch(std::exception &e)
            {
                UNUSED(e);
                EZMQ_LOG(ERROR, TAG, "Caught exception");
                return EZMQ_ERROR;
            }
        }
        VERIFY_NON_NULL(mContext)
        mStatus = EZMQ_Initialized;
        return EZMQ_OK;
    }

    EZMQErrorCode EZMQAPI::terminate()
    {
        EZMQ_SCOPE_LOGGER(TAG, __func__);
        mContext.reset();
        mStatus = EZMQ_Terminated;
        return EZMQ_OK;
    }

    EZMQStatusCode EZMQAPI::getStatus()
    {
        return mStatus;
    }

    std::shared_ptr<zmq::context_t>  EZMQAPI::getContext()
    {
        return mContext;
    }
}

