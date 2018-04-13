####### Building ezmq for windows platform [64-bit : amd64]:

Pre-requisites:
1. Python 2.7.0
2. SCons 2.5.1
3. 7-Zip
4. CMake 3.9.0
5. Visual Studio 15 update 3
   - https://my.visualstudio.com/Downloads?q=visual%20studio%202015&wt.mc_id=o~msft~vscom~older-downloads
   - On launching the installer select custom and in that select Visual C++.
6. Microsoft.NET [msbuild: v3.5]
   - Required for building ezmq dependencies: libzmq and protobuf.
   - Default path for msbuild tool is : C:/Windows/Microsoft.NET/Framework64/v3.5
   - Add above path to PATH.

Install dependencies from the web and add all to PATH variable.

Trouble-shooting:
1. If SCons can't find Python, try installing 32 bit version of Python.

Building ezmq library and sample:

Make sure all the pre-requisite tools are installed and added to PATH variable.

1. Open Visual studio terminal.
   - Terminal can be open like this:
    (a) Open window command line.
    (b) $ cd C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
    (c) $ call vcvarsall.bat amd64
        - The above script will turn cmd to visual studio terminal.

2. Building EMZQ dependencies:
   (a) libzmq:
       $ cd ~/protocol-ezmq-cpp/dependencies
       $ git clone https://github.com/zeromq/libzmq.git
       $ cd libzmq
       $ git checkout v4.2.2
       $ cd builds/msvc/vs2015
       $ MSBuild libzmq.sln /p:Configuration=StaticRelease

       On succesful build it will create libzmq.lib [./bin/x64/Release/v140/static/libzmq.lib]

   (b) protobuf:
       $ cd ~/protocol-ezmq-cpp/dependencies
       $ git clone https://github.com/google/protobuf.git
       $ cd protobuf
       $ git checkout v3.4.0
       $ cd ../
       $ rename protobuf protobuf-3.4.0
       $ cd protobuf-3.4.0
       $ git clone -b release-1.7.0 https://github.com/google/googlemock.git gmock
       $ cd gmock
       $ git clone -b release-1.7.0 https://github.com/google/googletest.git gtest
       $ cd ../cmake
       $ mkdir build & cd build
       $ mkdir solution & cd solution
       $ cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX=../../../../install ../..
       $ MSBuild protobuf.sln /p:Configuration=Release

       On succesful build it will create libzmq.lib [./cmake/build/solution/Release/libprotobuf.lib]

3. Build ezmq library:
    $ cd ~/protocol-ezmq-cpp/
    $ scons TARGET_OS=windows TARGET_ARCH=amd64

    Note:
    To build in debug mode use RELEASE=0 flag.

4. Run the samples:
   $ cd ~/protocol-ezmq-cpp/out/windows/win32/amd64/release/samples

   (a) Publisher:
       $ publisher.exe

   (b) Subscriber:
       $ subscriber.exe
