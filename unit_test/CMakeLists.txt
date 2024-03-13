set(UNIT_TEST "unit_test")
set(UNIT_TEST_SERVER "${UNIT_TEST}_server")
set(UNIT_TEST_CLIENT "${UNIT_TEST}_client")
set(UNIT_TEST_C_LIB_TEST "${UNIT_TEST}_c_lib_test")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/zmq_ipc/include
)

link_directories(${PROJECT_SOURCE_DIR}/output/lib)

# 服务端
add_executable(${UNIT_TEST_SERVER} server.cpp)
set_target_properties(${UNIT_TEST_SERVER} PROPERTIES
        LINKER_LANGUAGE CXX
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/bin
)
target_link_libraries(${UNIT_TEST_SERVER} zmq_ipc)

# 客户端
add_executable(${UNIT_TEST_CLIENT} client.cpp)
set_target_properties(${UNIT_TEST_CLIENT} PROPERTIES
        LINKER_LANGUAGE CXX
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/bin
)
target_link_libraries(${UNIT_TEST_CLIENT} zmq_ipc)

# 链接C库的客户端
add_executable(${UNIT_TEST_C_LIB_TEST} c_lib_test.c)
set_target_properties(${UNIT_TEST_C_LIB_TEST} PROPERTIES
        LINKER_LANGUAGE C
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/bin
)
target_link_libraries(${UNIT_TEST_C_LIB_TEST} czmq_ipc)
