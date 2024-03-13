cmake_minimum_required(VERSION 3.0.0)
project(cppzmq-ipc VERSION 0.1.2)

# 判断操作系统
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "current platform: Linux ")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "current platform: Windows")
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

    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

    # VS编译会报错：https://github.com/zeromq/libzmq/issues/4127
    set(BUILD_STATIC OFF)
    set(BUILD_SHARED ON)
endif ()

option(CROSS_COMPILE "Build for a different architecture" OFF)
if (CROSS_COMPILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/toolchain.cmake)
    message(STATUS "MYSYSROOT: " ${MYSYSROOT})
endif ()

# 加载第三方模块
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SubLibzmq)
include(SubCppzmq)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/zmq_proxy)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/zmq_ipc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unit_test)
