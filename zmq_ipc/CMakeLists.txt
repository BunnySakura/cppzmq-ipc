set(ZMQ_IPC "zmq_ipc")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/third_party/libzmq/include
        ${PROJECT_SOURCE_DIR}/third_party/cppzmq
)

link_directories(${PROJECT_SOURCE_DIR}/output/lib)

file(GLOB SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${ZMQ_IPC} SHARED ${SOURCE_CPP})
set_target_properties(${ZMQ_IPC} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${ZMQ_IPC} zmq)

# 依赖编译完成后，才可继续编译
add_dependencies(${ZMQ_IPC} libzmq)
