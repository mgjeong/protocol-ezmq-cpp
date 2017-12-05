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
