#!/bin/bash
./install_dependencies_armhf_qemu.sh

#build ezmq SDK and samples
scons TARGET_ARCH=armhf
