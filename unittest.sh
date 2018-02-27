#!/bin/bash
./build_auto.sh --target_arch=x86_64

cd out/linux/x86_64/release/unittests

#ezmq_api_test
./ezmq_api_test

#ezmq_pub_test
./ezmq_pub_test

#ezmq_sub_test
./ezmq_sub_test

#ezmq_byteData_test
./ezmq_byteData_test

