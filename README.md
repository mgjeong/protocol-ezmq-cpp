=========== C++ EZMQ guide ==========

############ Pre-requisite for buidling EZMQ SDK ############

 (I) Install ZeroMQ c binding:
    (a) echo "deb http://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/ ./" >> /etc/apt/sources.list
        Note: If permission denied error comes in this step, Follow the below instructions:
        - Goto /etc/apt/sources.list
        - Add the below line:
          deb http://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/ ./
        - Save and close it.

    (b) wget -q https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/Release.key  -O- | sudo apt-key add -
    (c) sudo apt-get install libzmq3-dev

    Above instructions are given at: http://zeromq.org/intro:get-the-software

 (II) cpp protocol buffer [protobuf]:
     (a) Protocol buffer version to be used for EZMQ is 3.4.0

     (b) Steps to install the protocol buffer:

         - Download the protobuf released code [3.4.0]:
          https://github.com/google/protobuf/releases

		 - Extract the zip/tar

        - Follow the build guide given at:
          ~/protobuf-3.4.0/src/README.md

############  Build instructions ############

(A)Using Linux command line:

1. Pre-requisutes:
   Scons should be installed on linux machine. check it using : scons -version
   Hint: $ sudo apt-get install scons

2. Goto: ~/EZMQ/cpp/

3. Build command:
   $ scons
   Note: (a) Supported flags are:
             - TARGET_OS
             - TARGET_ARCH
             - RELEASE
             - TEST
         (b) Supported OS is linux
         (c) To build in release mode use RELEASE=1
             e.g. scons RELEASE=1
         (d) To build in debug mode use RELEASE=0
             e.g: scons RELEASE=0

4. On successful build:
   - It will show following logs:
     scons: done building targets.

   - It will generate ezmq.so in following directory:
     ~/EZMQ/cpp/out/linux/{ARCH}/{MODE}/

   - It will build the sample application in following directory:
     ~/EZMQ/cpp/out/linux/{ARCH}/{MODE}/samples/

5. Run the subscriber sample application:
   - Goto: ~/EZMQ/cpp/out/linux/{ARCH}/{MODE}/samples/

   - export LD_LIBRARY_PATH=../

   - ./subscriber

   - On successful running it will show following logs:
       Initialize API [result]: 0
       Enter 1 for General Event testing
       Enter 2 for Topic Based delivery
       ctrl+c to terminate the program

6. Run the publisher sample application:
   - Goto: ~/EZMQ/cpp/out/linux/{ARCH}/{MODE}/samples/

   - export LD_LIBRARY_PATH=../

   - ./publisher

   - On successful running it will show following logs:
       Initialize API [result]: 0
       Enter 1 for General Event testing
       Enter 2 for Topic Based delivery

############  Test cases build and run instructions ############

1. Goto: ~/EZMQ/cpp/

2. Build the EZMQ CPP SDK with TEST=1 flag
   $scons TEST=1

   The above command will build and run the EZMQ unit test cases.

3. Test cases can also be run manually from Executables:
   - Goto: ~/EZMQ/cpp/out/linux/{ARCH}/{MODE}/unittests/

   - export LD_LIBRARY_PATH=../

   - Run the executables: ezmq_api_test, ezmq_pub_test and ezmq_sub_test

############ Unit test and Code coverage report generation ############

Pre-requisite:
1. Gcovr tool
   - http://gcovr.com/guide.html#installation

Instructions:
1. Goto: ~/EZMQ/cpp/

2. Run the script [generate_report.sh]:
   $ ./generate_report.sh

   Note: For getting help about script, use the following:
         $ ./generate_report.sh --help

3. On success, it will generate following reports in [~/EZMQ/cpp/] :
   (i)  UnitTestReport
   (ii) CoverageReport

############ Usage guide [For micro-services] ############

1. The micro-service which wants to use EZMQ APIs has to link following libraries:
   - ezmq.so
   - proto.so
   Refer sample application sconscript.

2. Refer EZMQ sample apps to use EZMQ SDK APIs. [~/EZMQ/cpp/samples]


########## Build EZMQ SDK for arm architecture [Cross compile on ubuntu] ################

1. Install tool-chain for arm:
   - sudo apt-get install gcc-arm-linux-gnueabi
   - sudo apt-get install g++-arm-linux-gnueabi

2. Build and install zeroMQ [libzmq] for arm architecture:

(a) git clone https://github.com/zeromq/libzmq.git
(b) $ cd libzmq
(c) git checkout v4.2.2
(d) chmod 777 version.sh
(e) chmod 777 autogen.sh
(f) ./autogen.sh
(g) ./configure --host=arm-none-linux-gnueabi CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++
(h) make -j 4
(i) sudo make install
(j) sudo ldconfig

3. Build protobuf lib for arm architecture:

(a) https://github.com/google/protobuf/releases [download protobuf 3.4.0]
(b) Unzip it
(c) chmod 777 autogen.sh && ./autogen.sh
    Note: If autogen.sh give error, add -k option to curl commands in autogen.sh
(d) ./configure --host=arm-linux CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++
(e) make
   - It will fail, when it try to run protoc arm executable on your machine. [Ignore the build error]
(f) sudo make install
   - It will fail, when it try to run protoc arm executable on your machine. [Ignore the build error]
(g) Copy libs and protoc compiler manually from : src/.libs folder
    (i)  goto src/.libs
         $ cd src/.libs
    (ii) copy libs and protoc:
         $ sudo cp protoc /usr/bin/
         $ sudo cp * /usr/local/lib/
(h) cd ../../
(i) sudo ldconfig

4. Goto: ~/cpp/

5. scons TARGET_ARCH=arm TC_PREFIX=/usr/bin/arm-linux-gnueabi- TC_PATH=/usr/bin/

Note:
1. Update flags as per your installation directories.

!!!!!! Run EZMQ samples [It will run on arm machine only]!!!!!!!!

1. Subscriber sample application:
   - Goto: ~/cpp/out/linux/{ARCH}/{MODE}/samples/
   - export LD_LIBRARY_PATH=../
   - ./subscriber

2. Publisher sample application:
   - Goto: ~/cpp/out/linux/{ARCH}/{MODE}/samples/
   - export LD_LIBRARY_PATH=../
   - ./publisher

########## Build EZMQ SDK for arm64 architecture [Cross compile on ubuntu] ################

1. Install tool-chain for arm64:
   $ sudo apt-get update
   $ sudo apt-get install gcc-4.8-aarch64-linux-gnu
   $ sudo apt-get install g++-4.8-aarch64-linux-gnu

2. Build and Install zeroMQ [libzmq] for arm64 architecture:

(a) git clone https://github.com/zeromq/libzmq.git
(b) $ cd libzmq
(c) chmod 777 version.sh
(d) chmod 777 autogen.sh
(e) ./autogen.sh
(f) ./configure --host=aarch64-unknown-linux-gnu CC=/usr/bin/aarch64-linux-gnu-gcc-4.8 CXX=/usr/bin/aarch64-linux-gnu-g++-4.8
(g) make -j 4
(h) sudo make install
(i) sudo ldconfig

3. Build protobuf lib for arm64 architecture:

(a) https://github.com/google/protobuf/releases [download protobuf 3.4.0]
(b) Unzip it
(c) chmod 777 autogen.sh && ./autogen.sh
    Note: If autogen.sh give error, add -k option to curl commands in autogen.sh
(d) ./configure --host=aarch64-unknown-linux-gnu CC=/usr/bin/aarch64-linux-gnu-gcc-4.8 CXX=/usr/bin/aarch64-linux-gnu-g++-4.8
(e) make
   - It will fail, when it try to run protoc arm64 executable on your machine. [Ignore the build error]
(f) sudo make install
   - It will fail, when it try to run protoc arm64 executable on your machine. [Ignore the build error]
(g) Copy libs and protoc compiler manually from : src/.libs folder
    (i)  goto src/.libs
         $ cd src/.libs
    (ii) copy libs and protoc:
         $ sudo cp protoc /usr/bin/
         $ sudo cp * /usr/local/lib/
(h) cd ../../
(i) sudo ldconfig

4. Goto: ~/cpp/

5. scons TARGET_ARCH=arm64 TC_PREFIX=/usr/bin/aarch64-linux-gnu- TC_PATH=/usr/bin/

Note:
1. Update flags as per your installation directories.

!!!!!! Run EZMQ samples [It will run on arm64 machine only] !!!!!!!!

1. Subscriber sample application:
   - Goto: ~/cpp/out/linux/{ARCH}/{MODE}/samples/
   - export LD_LIBRARY_PATH=../
   - ./subscriber

2. Publisher sample application:
   - Goto: ~/cpp/out/linux/{ARCH}/{MODE}/samples/
   - export LD_LIBRARY_PATH=../
   - ./publisher

########## Build EZMQ SDK for arm [hard float: armhf] architecture [Cross compile on ubuntu with static linking] ################

1. Install tool-chain for arm [hard float]:
   $ sudo apt-get update
   $ sudo apt-get install gcc-4.8-arm-linux-gnueabihf
   $ sudo apt-get install g++-4.8-arm-linux-gnueabihf

2. Build and install zeroMQ [libzmq] for arm [hard float] architecture:

(a) git clone https://github.com/zeromq/libzmq.git
(b) $ cd libzmq
(c) git checkout v4.2.2
(d) chmod 777 version.sh
(e) chmod 777 autogen.sh
(f) ./autogen.sh
(g) ./configure --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc-4.8 CXX=arm-linux-gnueabihf-g++-4.8 --disable-shared --enable-static CFLAGS=-fPIC CPPFLAGS=-fPIC
(h) make -j 4
(i) sudo make install
(j) sudo ldconfig

3. Build protobuf lib for arm [hard float] architecture:

(a) https://github.com/google/protobuf/releases [download protobuf 3.4.0]
(b) Unzip it
(c) chmod 777 autogen.sh && ./autogen.sh
    Note: If autogen.sh give error, add -k option to curl commands in autogen.sh
(d) ./configure --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc-4.8 CXX=arm-linux-gnueabihf-g++-4.8
(e) make
   - It will fail, when it try to run protoc arm executable on your machine. [Ignore the build error]
(f) sudo make install
   - It will fail, when it try to run protoc arm executable on your machine. [Ignore the build error]
(g) Copy libs and protoc compiler manually from : src/.libs folder
    (i)  goto src/.libs
         $ cd src/.libs
    (ii) copy libs and protoc:
         $ sudo cp protoc /usr/bin/
         $ sudo cp * /usr/local/lib/
(h) cd ../../
(i) sudo ldconfig

4. Goto: ~/cpp/

5. scons TARGET_ARCH=armhf TC_PREFIX=/usr/bin/arm-linux-gnueabihf- TC_PATH=/usr/bin/

Note:
1. Update flags as per your installation directories.

!!!!!! Run EZMQ samples [It will run on arm [hard float] machine only]!!!!!!!!

1. Subscriber sample application:
   - Goto: ~/cpp/out/linux/{ARCH}/{MODE}/samples/
   - export LD_LIBRARY_PATH=../
     Note: update export library path as per your library directory.
   - ./subscriber

2. Publisher sample application:
   - Goto: ~/cpp/out/linux/{ARCH}/{MODE}/samples/
   - export LD_LIBRARY_PATH=../
     Note: update export library path as per your library directory.
   - ./publisher
   
   
########## Cross compile ezmq samples for Raspberry pi board [static linking] ################

1. chmod build_armhf_static_samples.sh
2. ./build_armhf_static_samples.sh
3. On success, it will build ezmq samples for raspberry pi borad in following directory:
   ~/protocol-ezmq-cpp/out/linux/armhf/release/static_linked
4. Copy built samples [armhf_publisher & armhf_subscriber] to Raspberry pi board and run.
   $./armhf_publisher
   $./armhf_subscriber