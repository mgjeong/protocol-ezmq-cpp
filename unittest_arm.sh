#!/bin/bash
./build_auto.sh --target_arch=armhf-native

cd out/linux/armhf/release/unittests

#ezmq_api_test
./ezmq_api_test

#ezmq_pub_test
./ezmq_pub_test

#ezmq_sub_test
./ezmq_sub_test

#ezmq_byteData_test
./ezmq_byteData_test

