# ezMQ CPP Client SDK

protocol-ezmq-cpp library provides a standard messaging interface over various data streaming 
and serialization / deserialization middlewares along with some added functionalities.</br>
Following is the architecture of ezMQ client library: </br> </br>
![ezMQ Architecture](doc/images/ezMQ_architecture_0.1.png?raw=true "ezMQ Arch")

## Features:
* Currently supports streaming using 0mq and serialization / deserialization using protobuf.
* Publisher -> Multiple Subscribers broadcasting.
* Topic based subscription and data routing at source (read publisher).
* High speed serialization and deserialization.

## Future Work:
* High speed parallel ordered serialization / deserialization based on streaming load.
* Threadpool for multi-subscriber handling.
* Router pattern.
* Clustering Support.
</br></br>

## How to build ezmq library and samples
### pre-requisites
1. Scons should be installed on linux machine. </br>
   $ sudo apt-get install scons

### Build Instructions
1. Goto: ~/protocol-ezmq-cpp/
2. ./build.sh <options></br>
**Notes:** </br>
(i) For getting help about script: **$ ./build.sh --help** </br>
(ii)Currently, Script needs sudo permission for installing zeroMQ and protobuf libraries. In future need for sudo will be removed by installing those libraries in ezmq library.

## How to run ezmq samples

### pre-requisites
Built ezmq
### Run the subscriber sample application

1. Goto: ~/protocol-ezmq-cpp/out/linux/{ARCH}/{MODE}/samples/
2. export LD_LIBRARY_PATH=../
3. ./subscriber
4.  On successful running it will show following logs:

```
Initialize API [result]: 0
```
**Follow the instructions on the screen.**

###  Run the publisher sample application

1. Goto: ~/protocol-ezmq-cpp/out/linux/{ARCH}/{MODE}/samples/
2. export LD_LIBRARY_PATH=../
3. ./publisher
4. On successful running it will show following logs:

```
Initialize API [result]: 0
```
**Follow the instructions on the screen.**

##  ezmq Usage guide [For micro-services]

1. The micro-service which wants to use ezmq APIs has to link following libraries:</br></br>
   **(A) If micro-service wants to link ezmq dynamically following are the libraries it needs to link:**</br>
        - ezmq.so</br>
        - protobuf.so</br>
        - zmq.so </br> 
   **(B) If micro-service wants to link ezmq statically following are the libraries it needs to link:**</br>
        - ezmq.a</br>
        - protobuf.a</br>
        - zmq.a </br>
    
2. Refer ezmq sample apps for linking and ezmq library APIs usage. **[~/protocol-ezmq-cpp/samples]**
