set(UNIT_TEST "unit_test")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/zmq_ipc/include
)

link_directories(${PROJECT_SOURCE_DIR}/output/)

#file(GLOB SOURCE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable("${UNIT_TEST}_server" server.cpp)
set_target_properties("${UNIT_TEST}_server" PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries("${UNIT_TEST}_server" zmq_ipc)
set_target_properties("${UNIT_TEST}_server" PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/bin)

add_executable("${UNIT_TEST}_client" client.cpp)
set_target_properties("${UNIT_TEST}_client" PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries("${UNIT_TEST}_client" zmq_ipc)
set_target_properties("${UNIT_TEST}_client" PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/bin)
