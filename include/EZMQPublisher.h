/**
  * @file   EZMQPublisher.h
  *
  * @brief This file provides APIs for publisher: start, subscribe, stop.
  */

#ifndef EZMQ_PUBLISHER_H
#define EZMQ_PUBLISHER_H

#include "EZMQErrorCodes.h"

//Protobuf header file
#include "Event.pb.h"

//ZeroMQ header file
#include "zmq.hpp"

#include <list>
#include <mutex>

namespace ezmq
{
    /**
    * Callbacks to get error codes for start/stop of EZMQ publisher.
    * Note: As of now not being used.
    */
    typedef std::function<void(EZMQErrorCode code)> EZMQStartCB;
    typedef std::function<void(EZMQErrorCode code)> EZMQStopCB;
    typedef std::function<void(EZMQErrorCode code)> EZMQErrorCB;

    /**
    * @class  EZMQPublisher
    * @brief   This class Contains the APIs related to start, stop, publish APIs
    *               of EZMQ Publisher.
    */
    class EZMQPublisher
    {
        public:

            /**
            *  Construtor of EZMQPublisher.
            *
            * @param port - Port to be used for publisher socket.
            * @param startCB- Start callback.
            * @param stopCB - Stop Callback.
            * @param errorCB - Error Callback.
            */
            EZMQPublisher(int port, EZMQStartCB startCB, EZMQStopCB stopCB, EZMQErrorCB errorCB);

            /**
            * Destructor of EZMQPublisher.
            */
            ~EZMQPublisher();

            /**
            * Starts PUB instance.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode start();

            /**
            * Publish events on the socket for subscribers.
            *
            * @param event - event to be published.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode publish(ezmq::Event event);

            /**
            * Publish events on a specific topic on socket for subscribers.
            *
            * @param topic - Topic on which event needs to be published.
            * @param event - event to be published.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            * @note (1) Topic name should be as path format. For example:
            *       home/livingroom/ (2) Topic name can have letters [a-z, A-z],
            *       numerics [0-9] and special characters _ - . and /
            */
            EZMQErrorCode publish(std::string topic, ezmq::Event event);

            /**
            * Publish an events on list of topics on socket for subscribers. On any of
            * the topic in list, if it failed to publish event it will return
            * EZMQ_ERROR/EZMQ_INVALID_TOPIC.
            *
            * @param topic - List of Topics on which event needs to be published.
            * @param event - event to be published.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            *
            *  @note (1) Topic name should be as path format. For example:
            *       home/livingroom/ (2) Topic name can have letters [a-z, A-z],
            *       numerics [0-9] and special characters _ - . and /
            */
            EZMQErrorCode publish( std::list<std::string> topics, ezmq::Event event);

            /**
            * Stops PUB instance.
            *
            * @return EZMQErrorCode - EZMQ_OK on success, otherwise appropriate error code.
            */
            EZMQErrorCode stop();

            /**
            * Get the port of the publisher.
            *
            * @return port number as integer.
            */
            int getPort();

        private:
            int mPort;

            //callbacks
            EZMQStartCB mStartCallback;
            EZMQStopCB mStopCallback;
            EZMQErrorCB mErrorCallback;

            //ZMQ socket
            zmq::socket_t *mPublisher;
            std::shared_ptr<zmq::context_t> mContext;

            //Mutex
            std::recursive_mutex mPubLock;

            std::string getSocketAddress();
            std::string  sanitizeTopic(std::string topic);
            EZMQErrorCode syncClose();
    };
}
#endif //EZMQ_PUBLISHER_H