#!/bin/bash

USR_LIB_DIR=/usr/local/lib
USR_INC_DIR=/usr/local/include
KEYWORD_EZMQ_CPP=ezmq-cpp

# COPY shared library to /usr/local/lib
sudo cp out/linux/x86_64/release/libezmq.so /usr/local/lib

# COPY header file to /usr/local/include/opcua-c
sudo rm -rf $USR_INC_DIR/$KEYWORD_EZMQ_CPP
sudo mkdir $USR_INC_DIR/$KEYWORD_EZMQ_CPP
sudo cp -f ./include/* $USR_INC_DIR/$KEYWORD_EZMQ_CPP/
