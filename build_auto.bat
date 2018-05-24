@ECHO OFF
	set ezmq_cpp_dir=%cd%
	
    IF "%~1"=="" GOTO WITH_NO_FLAG	
	IF "%~1"=="--with_dependencies" GOTO WITH_FLAG
	GOTO ERROR_INVALID_PARAM
  
:BUILD_DEPENDENCY
	cd dependencies
	ECHO BUILDING DEPENDENCIES ZMQ AND PROTOBUF
	
	::Clone and build zmq if not already built.
	IF EXIST "libzmq" (
	ECHO LIBZMQ already exists. 
	) ELSE (
	::clone libzmq
	git clone https://github.com/zeromq/libzmq.git
	:: building libzmq
	IF EXIST "libzmq" (
	cd libzmq
	git checkout v4.2.2
	cd builds/msvc/vs2015
	MSBuild libzmq.sln /p:Configuration=StaticRelease
	)
	)
	:: Finished building libzmq
	
	cd %ezmq_cpp_dir%
	cd dependencies
	
	:: Build protobuf (Clone google-protobuf repo if not already installed.)
	IF EXIST "protobuf-3.4.0" (
	ECHO PROTOBUF already exists. 
	) ELSE (
	:: clone google-protobuf
	git clone https://github.com/google/protobuf.git
	IF EXIST "protobuf" (
	cd protobuf
	git checkout v3.4.0
	cd ../
	rename protobuf protobuf-3.4.0
	cd protobuf-3.4.0
	git clone -b release-1.7.0 https://github.com/google/googlemock.git gmock
	cd gmock
	git clone -b release-1.7.0 https://github.com/google/googletest.git gtest
	cd ../cmake
	mkdir build & cd build
	mkdir solution & cd solution
	cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX=../../../../install ../..
	MSBuild protobuf.sln /p:Configuration=Release
	)
	)
	:: Finished building protobuf
	
	GOTO BUILD_EZMQ_CPP

:WITH_FLAG
	IF "%~2"=="" GOTO ERROR_INVALID_PARAM_VALUE
	IF "%~2"=="true" GOTO BUILD_DEPENDENCY
	IF "%~2"=="True" GOTO BUILD_DEPENDENCY
	IF "%~2"=="false" GOTO WITH_NO_FLAG
	IF "%~2"=="False" GOTO WITH_NO_FLAG
	GOTO ERROR_INVALID_PARAM_VALUE
	
:WITH_NO_FLAG
    ECHO Dependency option not provided or set to false. Building with --with_dependencies=false.
    GOTO BUILD_EZMQ_CPP

:BUILD_EZMQ_CPP
    :: Build protocol-ezmq-cpp
	cd %ezmq_cpp_dir%
	call scons TARGET_OS=windows TARGET_ARCH=amd64
	GOTO END
	
:ERROR_INVALID_PARAM_VALUE
    ECHO The value of "%~1" was not found or INVALID. Please re run the batch file.
	ECHO e.g. windows.bat --with_dependencies=true
    GOTO END

:ERROR_INVALID_PARAM
	ECHO Invalid parameter provided. Please re run the batch file.
	ECHO e.g. windows.bat --with_dependencies=true
	GOTO END
	
:END