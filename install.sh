#!/bin/bash
sudo cp out/linux/x86_64/release/libezmq.so /usr/local/lib
cp -r include tmp
cp protobuf/* tmp/
sudo cp -r  tmp /usr/local/include/ezmq-cpp
