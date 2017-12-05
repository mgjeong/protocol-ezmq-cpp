#!/bin/bash
CPP_ROOT=$(pwd)
FILENAME="protobuf-cpp-3.4.0.tar.gz"
mkdir ./dependencies
cd ./dependencies

DEP_ROOT=$(pwd)
git clone https://github.com/zeromq/libzmq.git
cd libzmq
git checkout v4.2.2
chmod 777 version.sh
./version.sh
chmod 777 autogen.sh
./autogen.sh
./configure --host=aarch64-unknown-linux-gnu CC=/usr/bin/aarch64-linux-gnu-gcc-4.8 CXX=/usr/bin/aarch64-linux-gnu-g++-4.8

make -j 4
sudo make install
sudo ldconfig

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
./configure --host=aarch64-unknown-linux-gnu CC=/usr/bin/aarch64-linux-gnu-gcc-4.8 CXX=/usr/bin/aarch64-linux-gnu-g++-4.8
make -j 4
sudo make install
#handle error
sudo cp src/.libs/protoc /usr/bin
sudo cp src/.libs/* /usr/local/lib
sudo ldconfig
