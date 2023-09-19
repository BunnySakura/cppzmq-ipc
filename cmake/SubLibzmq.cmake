add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/libzmq)
set_target_properties(libzmq PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
