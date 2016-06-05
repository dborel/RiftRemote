cmake_policy (VERSION 2.8.3)
cmake_minimum_required (VERSION 2.8.3)

project (RiftRemote CXX)

set (LIBOVR_DIR 3rdParty/LibOVR/1.4.0)

include_directories ($LIBOVR_DIR/Include)

add_library (RiftRemote SHARED RiftRemote.cpp ${LIBOVR_DIR}/Src/OVR_CAPIShim.c)

target_link_libraries (RiftRemote User32)
