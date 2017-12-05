#!/bin/bash
./install_dependencies_arm.sh
scons TARGET_ARCH=armhf TC_PREFIX=/usr/bin/arm-linux-gnueabihf- TC_PATH=/usr/bin/
