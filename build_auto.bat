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

    GOTO DOWNLOAD_GTEST_HIPPMOCKS

:WITH_FLAG
    IF "%~2"=="" GOTO ERROR_INVALID_PARAM_VALUE
    IF "%~2"=="true" GOTO BUILD_DEPENDENCY
    IF "%~2"=="True" GOTO BUILD_DEPENDENCY
    IF "%~2"=="false" GOTO WITH_NO_FLAG
    IF "%~2"=="False" GOTO WITH_NO_FLAG
    GOTO ERROR_INVALID_PARAM_VALUE

:WITH_NO_FLAG
    ECHO Dependency option not provided or set to false. Building with --with_dependencies=false.
    ECHO Building in Release mode.
    GOTO DOWNLOAD_GTEST_HIPPMOCKS

:DOWNLOAD_GTEST_HIPPMOCKS
    ::gtest 1.7
    cd %ezmq_cpp_dir%/extlibs/gtest
    IF EXIST "googletest-release-1.7.0" (
    ECHO googletest-release-1.7.0 already exists.
    ) ELSE (
    git clone https://github.com/google/googletest.git
    rename googletest googletest-release-1.7.0
    cd googletest-release-1.7.0
    git checkout release-1.7.0
    )
    ::gtest 1.7 done

    ::hippmocks v5.0
    cd %ezmq_cpp_dir%/extlibs/hippomocks
    IF EXIST "hippomocks" (
    ECHO hippomocks already exists.
    ) ELSE (
    git clone https://github.com/dascandy/hippomocks.git
    cd hippomocks
    git checkout v5.0
    )
    ::hippmocks done

    GOTO BUILD_EZMQ_CPP

:BUILD_EZMQ_CPP
    :: Build protocol-ezmq-cpp
    cd %ezmq_cpp_dir%
	
	IF "%~3"=="" GOTO RELEASE_BUILD
	IF "%~3"=="--build_mode" (
	IF "%~4"=="" GOTO ERROR_INVALID_PARAM
	IF "%~4"=="Debug" GOTO DEBUG_BUILD
	IF "%~4"=="debug" GOTO DEBUG_BUILD
	)
	GOTO RELEASE_BUILD
	
:DEBUG_BUILD
	ECHO "BUILDING IN DEBUG MODE"
	call scons TARGET_OS=windows TARGET_ARCH=amd64 RELEASE=0
    GOTO END
	
:RELEASE_BUILD
	ECHO "BUILDING IN RELEASE MODE"
	call scons TARGET_OS=windows TARGET_ARCH=amd64
    GOTO END
	
:ERROR_INVALID_PARAM_VALUE
    ECHO The value of "%~1" was not found or INVALID. Please re run the batch file.
    ECHO e.g. build_auto.bat --with_dependencies=true
    GOTO END

:ERROR_INVALID_PARAM
    ECHO Invalid parameter provided. Please re run the batch file.
    ECHO e.g. build_auto.bat --with_dependencies=true
    ECHO e.g. build_auto.bat --with_dependencies=true --build_mode=debug
    GOTO END

:END

