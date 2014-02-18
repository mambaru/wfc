add_executable(reader0_test reader0_test.cpp)
add_test(reader0_test reader0_test)
target_link_libraries(reader0_test boost_system)

#add_executable(server_tcp_test server_tcp_test.cpp)
#add_executable(server_udp_test server_udp_test.cpp)

#target_link_libraries(server_tcp_test wfc boost_system pthread rt)
#target_link_libraries(server_udp_test wfc boost_system pthread rt)
