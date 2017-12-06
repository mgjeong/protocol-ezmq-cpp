#!/bin/bash
./install_dependencies_armhf.sh

#build ezmq SDK and samples
scons TARGET_ARCH=armhf TC_PREFIX=/usr/bin/arm-linux-gnueabihf- TC_PATH=/usr/bin/
