add_executable(jsonrpc_test  jsonrpc_test.cpp)
target_link_libraries(jsonrpc_test PUBLIC wfc)

wci_test(TARGET jsonrpc_test COMMAND jsonrpc_test)
