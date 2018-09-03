/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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
* @file   EZMQException.h
*
* @brief This file contains EZMQ exception class.
*/

#ifndef EZMQ_EXCEPTION_H
#define EZMQ_EXCEPTION_H

#include <string>

namespace ezmq
{
/**
* @class EZMQException
* Exception class that contains exception constructors and Message.
*/
class EZMQException : public std::exception
{
    public:
        /**
        * Constructor
        */
        EZMQException(){}

        /**
        * Constructor
        *
        * @param msg Exception message.
        */
        EZMQException(const std::string& msg);

        /**
        * Get exception message as char pointer.
        *
        * @return exception message as char pointer.
        */
        const char * what() const noexcept;

    private:
        std::string mMessage;
};

} // namespace ezmq

#endif //EZMQ_EXCEPTION_H

