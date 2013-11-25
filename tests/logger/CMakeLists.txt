add_executable(logstream_test logstream_test.cpp)
add_executable(logger_test logger_test.cpp ../../comet/core/global.cpp)
add_test(logstream_test logstream_test)
add_test(logger_test logger_test)

