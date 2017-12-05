# ==================== Entry point for scons ======================

# Load common build config
SConscript('build_common/SConscript')

Import('env')

# Get the build directory prefix
build_dir = env.get('BUILD_DIR')

# Build EZMQ SDK
SConscript(build_dir + 'SConscript')