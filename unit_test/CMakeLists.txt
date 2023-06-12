set(UNIT_TEST "unit_test")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/zmq_ipc/include
)

link_directories(${PROJECT_SOURCE_DIR}/output/)

file(GLOB SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${UNIT_TEST} ${SOURCE_CPP})
set_target_properties(${UNIT_TEST} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${UNIT_TEST} zmq_ipc)
