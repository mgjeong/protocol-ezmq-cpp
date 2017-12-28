# ezmq library (cpp)

protocol-ezmq-java is a library (jar) which provides a standard messaging interface over various data streaming
and serialization / deserialization middlewares along with some added functionalities.</br>
  - Currently supports streaming using 0mq and serialization / deserialization using protobuf.
  - Publisher -> Multiple Subscribers broadcasting.
  - Topic based subscription and data routing at source (read publisher).
  - High speed serialization and deserialization.


## Prerequisites ##
  - Remember, you must configure proxies for git and maven accordingly if necessary.
  - Setting up proxy for git
```shell
$ git config --global http.proxy http://proxyuser:proxypwd@proxyserver.com:8080
```
- JDK
  - Version : 1.8
  - [How to install](https://docs.oracle.com/javase/8/docs/technotes/guides/install/linux_jdk.html)
- Maven
  - Version : 3.5.2
  - [Where to download](https://maven.apache.org/download.cgi)
  - [How to install](https://maven.apache.org/install.html)
  - [Setting up proxy for maven](https://maven.apache.org/guides/mini/guide-proxies.html)


## How to build ##
1. Goto: ~/protocol-ezmq-cpp/
2. 
 ```
 ./build.sh <options></br>
 ```
**Notes** </br>
(a) For getting help about script: **$ ./build.sh --help** </br>
(b) Currently, Script needs sudo permission for installing zeroMQ and protobuf libraries. In future need for sudo will be removed by installing those libraries in ezmq library.

## How to run ##

### Prerequisites ###
 Built ezmq library

### Subscriber sample ###
1. Goto: ~/protocol-ezmq-cpp/out/linux/{ARCH}/{MODE}/samples/
2. export LD_LIBRARY_PATH=../
3. 
 ``` shall
 ./subscriber
 ```
4. On successful running it will show following logs:
 ``` 
 Initialize API [result]: 0
 ```
**Follow the instructions on the screen.**

### Publisher sample ###
1. Goto: ~/protocol-ezmq-cpp/out/linux/{ARCH}/{MODE}/samples/
2. export LD_LIBRARY_PATH=../
3. 
 ``` shall
 ./publisher
 ```
4. On successful running it will show following logs:
 ```
 Initialize API [result]: 0
 ```
**Follow the instructions on the screen.**

## Usage guide for ezmq library (For micro-services)

1. The micro-service which wants to use ezmq APIs has to link following libraries:</br></br>
   **(A) If micro-service wants to link ezmq dynamically following are the libraries it needs to link:**</br>
        - ezmq.so</br>
        - protobuf.so</br>
        - zmq.so </br> 
   **(B) If micro-service wants to link ezmq statically following are the libraries it needs to link:**</br>
        - ezmq.a</br>
        - protobuf.a</br>
        - zmq.a </br>
    
## Future Work ##
  - High speed parallel ordered serialization / deserialization based on streaming load.
  - Threadpool for multi-subscriber handling.
  - Router pattern. For number of subscribers to single publisher use case.
  - Clustering Support.
</br></br>
