add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/cppzmq)
set_target_properties(cppzmq PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
