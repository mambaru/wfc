add_executable(conn_test conn_test.cpp)
add_test(conn_test conn_test)
target_link_libraries(conn_test wfc boost_system)

#add_executable(server_tcp_test server_tcp_test.cpp)
#add_executable(server_udp_test server_udp_test.cpp)

#target_link_libraries(server_tcp_test wfc boost_system pthread rt)
#target_link_libraries(server_udp_test wfc boost_system pthread rt)
