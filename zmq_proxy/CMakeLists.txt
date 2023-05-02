set(ZMQ_PROXY "zmq_proxy")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/bin) # 设置可执行文件输出路径

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/third_party/libzmq/include
        ${PROJECT_SOURCE_DIR}/third_party/cppzmq
)

link_directories(${PROJECT_SOURCE_DIR}/output/lib)

file(GLOB SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${ZMQ_PROXY} ${SOURCE_CPP})
set_target_properties(${ZMQ_PROXY} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${ZMQ_PROXY} zmq)

# 等待依赖的链接库编译完成再继续
add_dependencies(${ZMQ_PROXY} libzmq)
