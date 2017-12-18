#!/bin/bash
./install_dependencies_arm.sh

#build ezmq SDK and samples
scons TARGET_ARCH=arm TC_PREFIX=/usr/bin/arm-linux-gnueabi- TC_PATH=/usr/bin/
