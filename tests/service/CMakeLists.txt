add_executable(service1_test service1_test.cpp logger.cpp)
add_test(service1_test service1_test)
target_link_libraries(service1_test wfc boost_system pthread)

add_executable(echo1bench echo1bench.cpp)
target_link_libraries(echo1bench pthread)