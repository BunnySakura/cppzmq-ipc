add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/libzmq)

# 根据不同平台链接库的目标类型设置输出路径
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(libzmq PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
    set_target_properties(libzmq PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
else ()
    set_target_properties(libzmq PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output/lib)
endif ()
