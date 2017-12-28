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

#ifndef EZMQ_LOGGER_H_
#define EZMQ_LOGGER_H_

#include <stdio.h>
#include <stdarg.h>
#ifdef __cplusplus
#include <cinttypes>
#else
#include <inttypes.h>
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

// Max buffer size
#define MAX_LOG_V_BUFFER_SIZE (256)

// Setting this flag for a log level means that the corresponding log message
// contains private data. This kind of message is logged only when a call to
// EZMQSetLogLevel() enabled private data logging.
#define EZMQ_LOG_PRIVATE_DATA (1 << 31)

typedef enum {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL
} LogLevel;

#define DEBUG_PRIVATE ((EZMQ_LOG_PRIVATE_DATA) | (DEBUG))
#define INFO_PRIVATE ((EZMQ_LOG_PRIVATE_DATA) | (INFO))
#define WARNING_PRIVATE ((EZMQ_LOG_PRIVATE_DATA) | (WARNING))
#define ERROR_PRIVATE ((EZMQ_LOG_PRIVATE_DATA) | (ERROR))
#define FATAL_PRIVATE ((EZMQ_LOG_PRIVATE_DATA) | (FATAL))

#ifndef EZMQ_LOG_LEVEL
#define EZMQ_MINIMUM_LOG_LEVEL    (DEBUG)
#else
#define EZMQ_MINIMUM_LOG_LEVEL    (EZMQ_LOG_LEVEL)
#endif

#define IF_EZMQ_PRINT_LOG_LEVEL(level) \
    if (((int)EZMQ_MINIMUM_LOG_LEVEL) <= ((int)(level & (~EZMQ_LOG_PRIVATE_DATA))))

/**
 * Set log level and privacy log to print.
 *
 * @param level - log level.
 * @param hidePrivateLogEntries - Hide Private Log.
 */
void EZMQSetLogLevel(LogLevel level, bool hidePrivateLogEntries);

/**
* Output a variable argument list log string with the specified priority level.
*
* @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
* @param tag    - Module name
* @param format - variadic log string
*/
void EZMQLogv(int level, const char * tag, const char * format, ...)
#if defined(__GNUC__)
    __attribute__ ((format(printf, 3, 4)))
#endif
    ;

/**
* Output a log string with the specified priority level.
*
* @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
* @param tag    - Module name
* @param logStr - log string
*/
void EZMQLog(int level, const char * tag, const char * logStr);

/**
* Output the contents of the specified buffer (in hex) with the specified priority level.
*
* @param level      - DEBUG, INFO, WARNING, ERROR, FATAL
* @param tag        - Module name
* @param buffer     - pointer to buffer of bytes
* @param bufferSize - max number of byte in buffer
*/
void EZMQLogBuffer(int level, const char* tag, const uint8_t* buffer, size_t bufferSize);


#ifdef DEBUG_LOG

#define EZMQ_LOG_BUFFER(level, tag, buffer, bufferSize) \
    do { \
        IF_EZMQ_PRINT_LOG_LEVEL((level)) \
            EZMQLogBuffer((level), (tag), (buffer), (bufferSize)); \
    } while(0)


#define EZMQ_LOG(level, tag, logStr) \
    do { \
        IF_EZMQ_PRINT_LOG_LEVEL((level)) \
            EZMQLog((level), (tag), (logStr)); \
    } while(0)

// Define variable argument log function
#define EZMQ_LOG_V(level, tag, ...) \
    do { \
        IF_EZMQ_PRINT_LOG_LEVEL((level)) \
            EZMQLogv((level), (tag), __VA_ARGS__); \
    } while(0)

#define EZMQ_SCOPE_LOGGER(TAG, FUNC) ScopeLogger scopeLogger(TAG, FUNC)

#else // DEBUG_LOG
#define EZMQ_LOG(level, tag, logStr)
#define EZMQ_LOG_V(level, tag, ...)
#define EZMQ_LOG_BUFFER(level, tag, buffer, bufferSize)
#define EZMQ_SCOPE_LOGGER(TAG, FUNC)
#endif // DEBUG_LOG

 namespace ezmq
{
    class ScopeLogger
    {
        public:
            ScopeLogger(const char *tag, const char *method)
            {
                m_funName = method;
                m_tag = tag;
                EZMQ_LOG_V(DEBUG, m_tag, "[%s] IN", m_funName);
            }

            ~ScopeLogger()
            {
                EZMQ_LOG_V(DEBUG, m_tag, "[%s] OUT", m_funName);
            }

        private:
            const char *m_funName;
            const char* m_tag;
    };
}

#endif // EZMQ_LOGGER_H_
