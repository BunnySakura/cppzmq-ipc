cmake_minimum_required(VERSION 3.0.0)
project(zmq_ipc_proj VERSION 0.1.0)

# 关闭测试
#set(ZMQ_BUILD_TESTS OFF)
#set(CPPZMQ_BUILD_TESTS OFF)

# 判断操作系统
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "current platform: Linux ")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "current platform: Windows")
    set(ENABLE_CURVE OFF) # win系统开启此设置会报错
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "current platform: Mac OS X")
else ()
    message(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

# 判断编译器
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(STATUS "using Clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "using GCC")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message(STATUS "using Intel C++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "using Visual Studio C++")
    # VS编译会报错：https://github.com/zeromq/libzmq/issues/4127
    set(BUILD_STATIC OFF)
    set(BUILD_SHARED ON)
endif ()

option(CROSS_COMPILE "Build for a different architecture" OFF)
if (CROSS_COMPILE)
    set(CMAKE_TOOLCHAIN_FILE ${PROJECT_SOURCE_DIR}/toolchain/toolchain.cmake)
    message(STATUS "MYSYSROOT: " ${MYSYSROOT})
endif ()

# 设置共享库输出路径
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
# 设置静态库输出路径
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

#add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/libzmq)
#add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/cppzmq)

add_subdirectory(${PROJECT_SOURCE_DIR}/zmq_proxy)
add_subdirectory(${PROJECT_SOURCE_DIR}/zmq_ipc)
