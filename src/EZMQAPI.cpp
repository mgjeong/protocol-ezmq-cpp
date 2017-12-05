
#include "EZMQAPI.h"
#include "EZMQLogger.h"

#include <iostream>

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
            mContext = std::make_shared<zmq::context_t>(1);
        }

        if(nullptr == mContext)
        {
            EZMQ_LOG(ERROR, TAG, "context is null");
            return EZMQ_ERROR;
        }
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
