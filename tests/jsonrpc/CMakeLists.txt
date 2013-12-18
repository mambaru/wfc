add_executable(server_test2 server_test.cpp)
#add_definitions(-DFAS_USE_RT_LIB)

target_link_libraries(server_test2 wfc boost_system pthread rt)
add_test(server_test2 server_test2)