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

## How to build 
**For building the ezMQ SDK following are the pre-requisites:**
1. Java 1.8 or higher <br>
2. Maven 3.5 or higher <br>

Build guide of **ezMQ sdk** is given [here](./edgex-ezmq/README.md)


## How to run sample

Build guide of **ezMQ samples** is given 
[here](./samples/README.md)
