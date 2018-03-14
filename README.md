# ezmq library (cpp)

protocol-ezmq-cpp is a library which provides a standard messaging interface over various data streaming
and serialization / deserialization middlewares along with some added functionalities.</br>
  - Currently supports streaming using 0mq and serialization / deserialization using protobuf.
  - Publisher -> Multiple Subscribers broadcasting.
  - Topic based subscription and data routing at source (read publisher).
  - High speed serialization and deserialization.


## Prerequisites ##
 - You must install basic prerequisites for build
   - Install build-essential
   ```
   $ sudo apt-get install build-essential
   ```

- Python
  - Version : 2.4 to 3.0
  - [How to install](https://wiki.python.org/moin/BeginnersGuide/Download)

- SCons
  - Version : 2.3.0 or above
  - [How to install](http://scons.org/doc/2.3.0/HTML/scons-user/c95.html)

## How to build ##
1. Goto: ~/protocol-ezmq-cpp/</br>
2. Following are the scripts for building ezmq library:</br>
   (a) build.sh      : Native build for x86_64</br>
   (b) build_arm.sh  : Cross build for armhf-qemu</br>
   (c) build_auto.sh : Generic script for building</br>

**Notes** </br>
(a) For getting help about script [ build_auto.sh ] : **$ ./build_auto.sh --help** </br>
(b) Currently, Script needs sudo permission for installing zeroMQ and protobuf libraries. In future need for sudo will be removed by installing those libraries in ezmq library. </br> 
(c) For native build on RPI [Raspberry Pi] Borad, use the following flags in build_auto.sh: </br>
    `$ /build_auto.sh --target_arch=armhf-native --with_dependencies=true``</br>
       - Update flags as required.

## How to run ##

### Prerequisites ###
 Built ezmq library

### Subscriber sample ###
1. Goto: ~/protocol-ezmq-cpp/out/linux/{ARCH}/{MODE}/samples/
2. export LD_LIBRARY_PATH=../
3. Run the sample:
    ```
     ./subscriber
    ```
    - **It will give list of options for running the sample.** </br>
    - **Update ip, port and topic as per requirement.** </br>

### Publisher sample ###
1. Goto: ~/protocol-ezmq-cpp/out/linux/{ARCH}/{MODE}/samples/
2. export LD_LIBRARY_PATH=../
3. Run the sample:
   ```
   ./publisher
   ```
   - **It will give list of options for running the sample.** </br>
   - **Update port and topic as per requirement.** </br>

## Usage guide for ezmq library (for microservices)

1. The microservice which wants to use ezmq APIs has to link following libraries:</br></br>
   **(A) If microservice wants to link ezmq dynamically following are the libraries it needs to link:**</br>
        - ezmq.so</br>
        - protobuf.so</br>
        - zmq.so </br>
   **(B) If microservice wants to link ezmq statically following are the libraries it needs to link:**</br>
        - ezmq.a</br>
        - protobuf.a</br>
        - zmq.a </br>
2. Reference ezmq library APIs : [doc/doxygen/docs/html/index.html](doc/doxygen/docs/html/index.html)

## Future Work ##
  - High speed parallel ordered serialization / deserialization based on streaming load.
  - Threadpool for multi-subscriber handling.
  - Router pattern. For number of subscribers to single publisher use case.
  - Clustering Support.
</br></br>
