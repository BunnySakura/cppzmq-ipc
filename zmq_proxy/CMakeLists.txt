set(ZMQ_PROXY "zmq_proxy")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/third_party/cppzmq
        ${PROJECT_SOURCE_DIR}/third_party/cxxopts/include
)

link_directories(${PROJECT_SOURCE_DIR}/output/bin)

file(GLOB SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${ZMQ_PROXY} ${SOURCE_CPP})
set_target_properties(${ZMQ_PROXY} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${ZMQ_PROXY} cppzmq)

set_target_properties(${ZMQ_PROXY} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/bin)
