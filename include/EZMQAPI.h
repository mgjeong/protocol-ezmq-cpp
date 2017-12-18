/**
  * @file   EZMQAPI.h
  *
  * @brief This file provides APIs for EZMQ: initialize, terminate.
  */

#ifndef EZMQ_API_H
#define EZMQ_API_H

#include "EZMQErrorCodes.h"

//ZeroMQ header file
#include "zmq.hpp"

#include <memory>

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
              std::srand(std::time(0));
            }
            EZMQStatusCode mStatus;
            std::shared_ptr<zmq::context_t> mContext;
    };
}
#endif  //EZMQ_API_H