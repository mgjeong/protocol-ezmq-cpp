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
 * @file   EZMQErrorCodes.h
 *
 * @brief   This file contains error codes of EZMQ stack possibly return to application.
 */
#ifndef EZMQ_ERROR_CODES_H_
#define EZMQ_ERROR_CODES_H_

namespace ezmq
{
    /**
    * @enum EZMQErrorCode
    * EZMQ stack error codes.
    */
    typedef enum
    {
        EZMQ_OK = 0,
        EZMQ_ERROR,
        EZMQ_INVALID_TOPIC
    } EZMQErrorCode;

    /**
    * @enum EZMQStatusCode
    * EZMQ Service status codes.
    */
    typedef enum
    {
        EZMQ_Unknown = 0,
        EZMQ_Constructed,
        EZMQ_Initialized,
        EZMQ_Terminated
    } EZMQStatusCode;
}
#endif  // EZMQ_ERROR_CODES_H_

