add_executable(comet_test comet_test.cpp)
#link_directories(../../build/comet)
target_link_libraries(comet_test comet)
add_test(comet_test comet_test)