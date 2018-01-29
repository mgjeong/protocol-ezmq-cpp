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

#ifndef EZMQ_MESSAGE_H_
#define EZMQ_MESSAGE_H_

#include <iostream>

#include "EZMQErrorCodes.h"

namespace ezmq
{
    /**
     * @enum EZMQContentType
     * EZMQ message's content types.
     */
    typedef enum
    {
        EZMQ_CONTENT_TYPE_PROTOBUF = 0,
        EZMQ_CONTENT_TYPE_BYTEDATA,
        EZMQ_CONTENT_TYPE_AML,  //Not in use as of now
        EZMQ_CONTENT_TYPE_JSON  //Not in use as of now
    } EZMQContentType;

    class EZMQMessage
    {
        public:
        friend class EZMQSubscriber;
        friend class EZMQPublisher;

        virtual ~EZMQMessage() = default;

        /**
         * Get content-type of EZMQMessage.
         *
         * @return content type.
         */
        EZMQContentType getContentType() const;

        /**
         * Set content-type of EZMQMessage.
         * For EZMQMessage's protobuf construtor, default value will be EZMQ_CONTENT_TYPE_PROTOBUF.
         * For EZMQMessage's byte data construtor, default value will be EZMQ_CONTENT_TYPE_BYTEDATA.
         *
         * @param type - Content type to be set.
         *
         * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
         */
        EZMQErrorCode setContentType(EZMQContentType type);

        /**
         * Get version of EZMQ service.
         *
         * @return EZMQ version.
         */
        std::string getEZMQVersion() const;

        private:
            std::string mVersion;
            EZMQContentType mContentType;
    };
}

#endif // EZMQ_MESSAGE_H_

