cmake_minimum_required(VERSION 3.0.0)
project(zmq_ipc_proj VERSION 0.1.0)

set(BUILD_TESTS OFF)

option(CROSS_COMPILE "Build for a different architecture" OFF)
if (CROSS_COMPILE)
    set(CMAKE_TOOLCHAIN_FILE ${PROJECT_SOURCE_DIR}/toolchain/toolchain.cmake)
    message(STATUS "MYSYSROOT: " ${MYSYSROOT})
endif ()

# 设置共享库输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
# 设置静态库输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/libzmq)
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/cppzmq)

add_subdirectory(${PROJECT_SOURCE_DIR}/zmq_ipc)
add_subdirectory(${PROJECT_SOURCE_DIR}/zmq_proxy)
