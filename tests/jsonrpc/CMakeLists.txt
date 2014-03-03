add_executable(jsonrpc1_test jsonrpc1_test.cpp)
#add_definitions(-DFAS_USE_RT_LIB)

target_link_libraries(jsonrpc1_test wfc boost_system pthread rt)
add_test(jsonrpc1_test jsonrpc1_test)