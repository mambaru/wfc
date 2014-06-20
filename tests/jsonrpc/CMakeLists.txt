add_executable(jsonrpc1_test jsonrpc1_test.cpp)
add_dependencies(jsonrpc1_test wfc)
target_link_libraries(jsonrpc1_test wfc boost_system pthread rt)
add_test(jsonrpc1_test jsonrpc1_test)

add_executable(jsonrpc2_test jsonrpc2_test.cpp)
add_dependencies(jsonrpc2_test wfc)
target_link_libraries(jsonrpc2_test wfc boost_system pthread rt)
add_test(jsonrpc2_test jsonrpc2_test)