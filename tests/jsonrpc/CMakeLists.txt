add_executable(jsonrpc_test  jsonrpc_test.cpp)
target_link_libraries(jsonrpc_test wfc)

add_test(
  NAME jsonrpc_test
  COMMAND jsonrpc_test
)
