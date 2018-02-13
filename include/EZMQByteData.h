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

#ifndef EZMQ_BYTEDATA_H_
#define EZMQ_BYTEDATA_H_

#include "EZMQMessage.h"

namespace ezmq
{
    class EZMQByteData : public EZMQMessage
    {
        public:
            friend class EZMQSubscriber;
            friend class EZMQPublisher;

            /**
             * Construtor for EZMQByteData.
             *
             * @param data - Byte data.
             * @param dataLength - Data length.
             */
            EZMQByteData(const uint8_t *data,  size_t dataLength);

            /**
             * Destructor of EZMQByteData.
             */
            ~EZMQByteData();

            /**
             * Get length of data.
             *
             * @return Length of data.
             */
            size_t getLength() const;

            /**
             * Get message's byte data.
             *
             * @return data - byte data.
             */
            const uint8_t * getByteData() const;

            /**
             * Set byte data.
             * This method can be used to update data
             * of already created EZMQByteData object.
             *
             * @return data - byte data.
             */
            EZMQErrorCode setByteData(const uint8_t * data, size_t dataLength);

        private:
            const uint8_t *mData;
            size_t mDataLength;
    };
}

#endif // EZMQ_BYTEDATA_H_

