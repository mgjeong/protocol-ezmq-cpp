#!/bin/bash

#install required cross compile tool-chain
sudo apt-get update
sudo apt-get install gcc-arm-linux-gnueabi
sudo apt-get install g++-arm-linux-gnueabi

mkdir ./dependencies
cd ./dependencies
DEP_ROOT=$(pwd)

#Clone, build and install ZeroMQ library for arm architecture
git clone https://github.com/zeromq/libzmq.git
cd libzmq
git checkout v4.2.2
chmod 777 version.sh
./version.sh
chmod 777 autogen.sh
./autogen.sh
./configure --host=arm-none-linux-gnueabi CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++
make -j 4
sudo make install
sudo ldconfig

#build, install protobuf library for arm architecture
FILENAME="protobuf-cpp-3.4.0.tar.gz"
cd $DEP_ROOT
if [ -e"$FILENAME" ] ; then
    echo "file exist"
else
    wget https://github.com/google/protobuf/releases/download/v3.4.0/protobuf-cpp-3.4.0.tar.gz
fi
    rm -rf protobuf-3.4.0/
tar -xvf protobuf-cpp-3.4.0.tar.gz
cd protobuf-3.4.0/
chmod 777 autogen.sh
./autogen.sh
./configure --host=arm-linux CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++
make -j 4
sudo make install
#handle error
sudo cp src/.libs/protoc /usr/bin
sudo cp src/.libs/* /usr/local/lib
sudo ldconfig
