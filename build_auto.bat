@ECHO OFF
    set ezmq_cpp_dir=%cd%
	set dependencies=false
	set buildmode=release
	set secure=false

    IF "%~1"=="" GOTO BUILD_EZMQ_CPP
	
	:: Dependency flag check
    IF "%~1"=="--with_dependencies" (
		IF "%~2"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~2"=="true" (
		set dependencies=true
		)
	) ELSE IF "%~3"=="--with_dependencies" (
		IF "%~4"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~4"=="true" (
		set dependencies=true
		)
	)ELSE IF "%~5"=="--with_dependencies" (
		IF "%~6"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~6"=="true" (
		set dependencies=true
		)
	)
	
	:: Security flag check
    IF "%~1"=="--with_security" (
		IF "%~2"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~2"=="true" (
		set secure=true
		)
	) ELSE IF "%~3"=="--with_security" (
		IF "%~4"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~4"=="true" (
		set secure=true
		)
	)ELSE IF "%~5"=="--with_security" (
		IF "%~6"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~6"=="true" (
		set secure=true
		)
	)
	
	:: Build mode flag check
    IF "%~1"=="--build_mode" (
		IF "%~2"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~2"=="debug" (
		set buildmode=debug
		)
	) ELSE IF "%~3"=="--build_mode" (
		IF "%~4"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~4"=="debug" (
		set buildmode=debug
		)
	)ELSE IF "%~5"=="--build_mode" (
		IF "%~6"=="" GOTO ERROR_INVALID_PARAM
		
		IF "%~6"=="debug" (
		set buildmode=debug
		)
	)
	
	if %dependencies%==true (
	GOTO BUILD_DEPENDENCY
	) 
	
    GOTO BUILD_EZMQ_CPP

:BUILD_DEPENDENCY
    if %secure%==true (
	ECHO SECURE MODE ON, CLONING LIBSODIUM
	cd dependencies
	
	::Clone and build libsodium if not already built.
	IF EXIST "libsodium" (
	ECHO LIBSODIUM already exists.
	) ELSE (
	::clone libsodium
	git clone https://github.com/jedisct1/libsodium.git
	:: building libsodium
	IF EXIST "libsodium" (
	cd libsodium
	git checkout 1.0.16
	cd builds/msvc/vs2015
	MSBuild libsodium.sln /p:Configuration=StaticRelease
	)
	)
	ECHO Finished building libsodium
	)

    cd %ezmq_cpp_dir%
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
	if %secure%==true (
	MSBuild libzmq.sln /p:Configuration=StaticRelease
	) ELSE (
	MSBuild libzmq.sln /p:Configuration=StaticRelease,Option-sodium=true
	)
    )
    )
    ECHO Finished building libzmq

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

:BUILD_EZMQ_CPP
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

    :: Build protocol-ezmq-cpp
    cd %ezmq_cpp_dir%
	
	IF %buildmode%==debug (
	GOTO DEBUG_BUILD
	)
	
	GOTO RELEASE_BUILD
	
:DEBUG_BUILD
	ECHO "BUILDING IN DEBUG MODE"
	IF %secure%==true (
	call scons TARGET_OS=windows TARGET_ARCH=amd64 RELEASE=0 SECURED=1
	) ELSE (
	call scons TARGET_OS=windows TARGET_ARCH=amd64 RELEASE=0
	)
	
    GOTO END
	
:RELEASE_BUILD
	ECHO "BUILDING IN RELEASE MODE"
	IF %secure%==true (
	call scons TARGET_OS=windows TARGET_ARCH=amd64 SECURED=1
	) ELSE (
	call scons TARGET_OS=windows TARGET_ARCH=amd64
	)
	
    GOTO END

:ERROR_INVALID_PARAM
    ECHO Invalid parameter provided. Please re run the batch file.
    ECHO e.g. build_auto.bat --with_dependencies=true
    ECHO e.g. build_auto.bat --with_dependencies=true --build_mode=debug
	ECHO e.g. build_auto.bat --with_dependencies=true --with_security=true --build_mode=debug
	ECHO e.g. build_auto.bat --with_security=true
    GOTO END

:END

