add_executable(asio_test asio_test.cpp)
#link_directories(../../build/comet)
target_link_libraries(asio_test comet boost_system pthread)
add_test(asio_test asio_test)