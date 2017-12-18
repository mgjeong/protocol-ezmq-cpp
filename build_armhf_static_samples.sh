#!/bin/bash
./install_dependencies_armhf.sh

#build ezmq SDK and samples [It will build samples with dynamic linking]
scons TARGET_ARCH=armhf TC_PREFIX=/usr/bin/arm-linux-gnueabihf- TC_PATH=/usr/bin/

EZMQ_ROOT=$(pwd)
#Make a new folder static_libraries and copy libs [ezmq.a, zmq.a and protobuf.a] and samples to build in it
cd ./out/linux/armhf/release/
mkdir static_linked
cd static_linked
cp /usr/local/lib/libprotobuf.a .
cp /usr/local/lib/libzmq.a .
cp ../libezmq.a .
cp $EZMQ_ROOT/samples/publisher.cpp .
cp $EZMQ_ROOT/samples/subscriber.cpp .

echo $PWD 

#build ezmq samples [It will build samples with static linking to libezmq, libzmq, libprotobuf]
arm-linux-gnueabihf-g++-4.8 -std=c++11 -I/usr/local/include/ -I$EZMQ_ROOT/protobuf/  -I$EZMQ_ROOT/extlibs/zmq/ -I$EZMQ_ROOT/include/ -L /$PWD $EZMQ_ROOT/samples/publisher.cpp -o armhf_publisher -lezmq  -lzmq -lprotobuf -lpthread

arm-linux-gnueabihf-g++-4.8 -std=c++11 -I/usr/local/include/ -I$EZMQ_ROOT/protobuf/  -I$EZMQ_ROOT/extlibs/zmq/ -I$EZMQ_ROOT/include/ -L$PWD $EZMQ_ROOT/samples/subscriber.cpp -o armhf_subscriber  -lezmq -lzmq -lprotobuf -lpthread

