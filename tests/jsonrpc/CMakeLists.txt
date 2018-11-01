add_executable(jsonrpc_test  jsonrpc_test.cpp)
target_link_libraries(jsonrpc_test ${wfc_liblist})

add_test(
  NAME jsonrpc_test
  COMMAND jsonrpc_test
  WORKING_DIRECTORY ${WFC_TEST_DIR} 
)
