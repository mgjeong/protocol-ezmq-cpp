#!/bin/bash
./install_dependencies_arm64.sh

#build ezmq SDK and samples
scons TARGET_ARCH=arm64 TC_PREFIX=/usr/bin/aarch64-linux-gnu- TC_PATH=/usr/bin/
