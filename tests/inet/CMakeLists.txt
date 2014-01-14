add_executable(server_tcp_test server_tcp_test.cpp)
add_executable(server_udp_test server_udp_test.cpp)
#add_definitions(-DFAS_USE_RT_LIB)

target_link_libraries(server_tcp_test wfc boost_system pthread rt)
target_link_libraries(server_udp_test wfc boost_system pthread rt)
#add_test(server_test server_test)