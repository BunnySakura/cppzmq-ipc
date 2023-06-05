set(ZMQ_PROXY "zmq_proxy")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

link_directories(${PROJECT_SOURCE_DIR}/output/)

file(GLOB SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${ZMQ_PROXY} ${SOURCE_CPP})
set_target_properties(${ZMQ_PROXY} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${ZMQ_PROXY} cppzmq)
