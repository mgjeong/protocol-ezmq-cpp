###############################################################################
# Copyright 2017 Samsung Electronics All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###############################################################################

################ EZMQ build script ##################

import os
Import('env')

ezmq_env = env.Clone()
target_os = ezmq_env.get('TARGET_OS')
target_arch = ezmq_env.get('TARGET_ARCH')

ezmq_env.AppendUnique(CPPPATH=[
        './extlibs/zmq',
        './extlibs/hippomocks/hippomocks',
        './protobuf',
        './include',
        './include/logger',
        './src',
        './dependencies/libzmq/include',
        './dependencies/protobuf-3.4.0/src/',
])

if target_os not in ['windows']:
    ezmq_env.PrependUnique(LIBS=['zmq', 'protobuf'])
    ezmq_env.AppendUnique(
        CXXFLAGS=['-O2', '-g', '-Wall', '-fPIC', '-fmessage-length=0', '-std=c++0x', '-I/usr/local/include'])
    ezmq_env.AppendUnique(LINKFLAGS=['-Wl,--no-undefined'])
    ezmq_env.AppendUnique(LIBS=['pthread'])
    if not env.get('RELEASE'):
        ezmq_env.AppendUnique(CCFLAGS=['-g'])
        ezmq_env.PrependUnique(LIBS=['gcov'])
        ezmq_env.AppendUnique(CXXFLAGS=['--coverage'])
    else:
        ezmq_env.AppendUnique(CCFLAGS=['-Os'])
else:
    ezmq_env.Append(LIBS=File('#/dependencies/libzmq/bin/x64/Release/v140/static/libzmq.lib'))
    ezmq_env.Append(LIBS=File('#/dependencies/protobuf-3.4.0/cmake/build/solution/Release/libprotobuf.lib'))

EZMQ_DIR = '.'
ezmq_env.AppendUnique(ezmq_src = [ezmq_env.Glob(os.path.join(EZMQ_DIR, 'src', '*.cpp')),
                                ezmq_env.Glob(os.path.join(EZMQ_DIR, 'src', 'logger', '*.cpp')),
                                ezmq_env.Glob(os.path.join(EZMQ_DIR, 'protobuf', '*.cc'))])

if target_os == 'linux':
    ezmqshared = ezmq_env.SharedLibrary('ezmq', ezmq_env.get('ezmq_src'))
ezmqstatic = ezmq_env.StaticLibrary('ezmq', ezmq_env.get('ezmq_src'))

# Go to build EZMQ sample apps
if target_os in ['linux', 'windows']:
       SConscript('samples/SConscript')

# Go to build EZMQ unit test cases
if target_os == 'linux':
    if target_arch in ['x86', 'x86_64', 'armhf']:
        SConscript('unittests/SConscript')

if target_os == 'windows':
    SConscript('unittests/SConscript')
