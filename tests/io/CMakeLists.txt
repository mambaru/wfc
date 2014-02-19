add_executable(reader0_test reader0_test.cpp)
add_test(reader0_test reader0_test)
target_link_libraries(reader0_test boost_system pthread)


add_executable(reader1_test reader1_test.cpp)
add_test(reader1_test reader1_test)
target_link_libraries(reader1_test boost_system pthread)

#add_executable(server_tcp_test server_tcp_test.cpp)
#add_executable(server_udp_test server_udp_test.cpp)

#target_link_libraries(server_tcp_test wfc boost_system pthread rt)
#target_link_libraries(server_udp_test wfc boost_system pthread rt)
