################ EZMQ build script ##################

import os

Import('env')

ezmq_env = env.Clone()
target_os = ezmq_env.get('TARGET_OS')
target_arch = ezmq_env.get('TARGET_ARCH')

if ezmq_env.get('RELEASE'):
    ezmq_env.AppendUnique(CCFLAGS=['-Os'])
else:
    ezmq_env.AppendUnique(CCFLAGS=['-g'])

ezmq_env.AppendUnique(CPPPATH=[
         './extlibs/zmq',
        './protobuf',
        './include',
        './include/logger',
        './src'
])

ezmq_env.PrependUnique(LIBS=['zmq', 'protobuf'])

if target_os not in ['windows']:
    ezmq_env.AppendUnique(
        CXXFLAGS=['-O2', '-g', '-Wall', '-fPIC', '-fmessage-length=0', '-std=c++0x', '-I/usr/local/include'])

if target_os not in ['windows']:
    ezmq_env.AppendUnique(LINKFLAGS=['-Wl,--no-undefined'])

if target_os in ['linux']:
    ezmq_env.AppendUnique(LIBS=['pthread'])

if target_os in ['linux']:
    if not env.get('RELEASE'):
        ezmq_env.PrependUnique(LIBS=['gcov'])
        ezmq_env.AppendUnique(CXXFLAGS=['--coverage'])

EZMQ_DIR = '.'
ezmq_env.AppendUnique(ezmq_src = [ezmq_env.Glob(os.path.join(EZMQ_DIR, 'src', '*.cpp')),
                                ezmq_env.Glob(os.path.join(EZMQ_DIR, 'src', 'logger', '*.cpp')),
                                ezmq_env.Glob(os.path.join(EZMQ_DIR, 'protobuf', '*.cc'))])

ezmqshared = ezmq_env.SharedLibrary('ezmq', ezmq_env.get('ezmq_src'))
ezmqstatic = ezmq_env.StaticLibrary('ezmq', ezmq_env.get('ezmq_src'))

# Go to build EZMQ sample apps
if target_os == 'linux':
       SConscript('samples/SConscript')

# Go to build EZMQ unit test cases
if target_os == 'linux':
    if target_arch in ['x86', 'x86_64']:
        SConscript('unittests/SConscript')
