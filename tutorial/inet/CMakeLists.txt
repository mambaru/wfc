add_executable(inet1-srv inet1-srv.cpp)


target_link_libraries(inet1-srv boost_system pthread)
#target_link_libraries(server_tcp_test wfc boost_system pthread rt)
#target_link_libraries(server_udp_test wfc boost_system pthread rt)
