#!/bin/bash

USR_LIB_DIR=/usr/local/lib
USR_INC_DIR=/usr/local/include
KEYWORD_EZMQ_CPP=protocol-ezmq-cpp

# COPY shared library to /usr/local/lib
sudo cp out/linux/armhf/release/libezmq.so /usr/local/lib

# COPY header file to /usr/local/include/protocol-ezmq-cpp
sudo rm -rf $USR_INC_DIR/$KEYWORD_EZMQ_CPP
sudo mkdir $USR_INC_DIR/$KEYWORD_EZMQ_CPP
mkdir tmp
cp -rf ./protobuf/* ./tmp/
cp -rf ./include/* ./tmp/
sudo cp -rf ./tmp/* $USR_INC_DIR/$KEYWORD_EZMQ_CPP/
rm -rf ./tmp
