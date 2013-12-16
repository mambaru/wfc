add_executable(server_test server_test.cpp)
#add_definitions(-DFAS_USE_RT_LIB)

target_link_libraries(server_test wfc boost_system pthread rt)
add_test(server_test server_test)