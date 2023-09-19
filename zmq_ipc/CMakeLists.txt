set(ZMQ_IPC "zmq_ipc")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/third_party/ThreadPool
)

link_directories(${PROJECT_SOURCE_DIR}/output/lib)

file(GLOB SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${ZMQ_IPC} SHARED ${SOURCE_CPP})
set_target_properties(${ZMQ_IPC} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${ZMQ_IPC} cppzmq)

# 根据不同平台链接库的目标类型设置输出路径
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(${ZMQ_IPC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
    set_target_properties(${ZMQ_IPC} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
else ()
    set_target_properties(${ZMQ_IPC} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
endif ()
