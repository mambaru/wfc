set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})

enable_testing()
add_subdirectory(module)
add_subdirectory(jsonrpc)

