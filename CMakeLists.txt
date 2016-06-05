cmake_policy (VERSION 2.8.3)
cmake_minimum_required (VERSION 2.8.3)

project (RiftRemote CXX)

set (LIBOVR_DIR 3rdParty/LibOVR/1.4.0)

include_directories (${LIBOVR_DIR}/Include)

set (SHIM_FILE ${LIBOVR_DIR}/Src/OVR_CAPIShim.c)
add_executable (RiftRemote RiftRemote.cpp ${SHIM_FILE})
SET_SOURCE_FILES_PROPERTIES(${LIBOVR_DIR}/Src/OVR_CAPIShim.c PROPERTIES LANGUAGE CXX)

target_link_libraries (RiftRemote User32)
