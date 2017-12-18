#!/bin/bash
mkdir ./dependencies
cd ./dependencies
DEP_ROOT=$(pwd)

#Clone, build and install ZeroMQ library
git clone https://github.com/zeromq/libzmq.git
cd libzmq
git checkout v4.2.2
chmod 777 version.sh
./version.sh
chmod 777 autogen.sh
./autogen.sh
./configure
make -j 4
sudo make install
sudo ldconfig

#build, install protobuf library
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
./configure
make -j 4
sudo make install
sudo ldconfig
