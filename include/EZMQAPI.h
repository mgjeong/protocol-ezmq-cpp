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

/**
  * @file   EZMQAPI.h
  *
  * @brief This file provides APIs for EZMQ: initialize, terminate.
  */

#ifndef EZMQ_API_H
#define EZMQ_API_H

#include <memory>
#include <time.h>

#if defined(_WIN32)
#define ZMQ_STATIC
#pragma once
#include "zmq.hpp"
#undef ZMQ_STATIC
#else
#include "zmq.hpp"
#endif

#include "EZMQErrorCodes.h"

namespace ezmq
{
    /**
    * @class  EZMQAPI
    * @brief   This class Contains the APIs related to initialization, termination
    *               of EZMQ stack.
    */
    class EZMQAPI
    {
        public:
            /**
            * Get instance of EZMQAPI Class.
            *
            * @return Instance of EZMQAPI.
            */
            static EZMQAPI *getInstance();

            /**
            * Initialize required EZMQ components.
            * This API should be called first, before using any EZMQ APIs.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode initialize();

            /**
            * Perform cleanup of EZMQ components.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode terminate();

            /**
            * Get status of EZMQ service.
            *
            * @return EZMQStatusCode - Current status of EZMQ Service.
            */
            EZMQStatusCode getStatus();

            // For EZMQ internal use
            std::shared_ptr<zmq::context_t>  getContext();

        private:
            EZMQAPI()
            {
                mStatus = EZMQ_Constructed;
                std::srand(time(0));
            }
            EZMQStatusCode mStatus;
            std::shared_ptr<zmq::context_t> mContext;
    };
}
#endif  //EZMQ_API_H

