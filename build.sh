#!/bin/bash
./install_dependencies.sh

#build ezmq SDK and samples as per machine architecture
MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
  echo 64 bits
  scons TARGET_OS=linux TARGET_ARCH=x86_64
else
  echo 32 bits
  scons TARGET_OS=linux TARGET_ARCH=x86
fi


