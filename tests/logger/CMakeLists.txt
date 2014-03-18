
find_package( Threads )

add_executable(logstream_test logstream_test.cpp)
add_executable(logger_test logger_test.cpp ../../wfc/core/global.cpp)
target_link_libraries(logger_test boost_system ${CMAKE_THREAD_LIBS_INIT})
add_test(logstream_test logstream_test)
add_test(logger_test logger_test)

