add_executable(pubsub_test pubsub_test.cpp)
target_link_libraries(pubsub_test comet)
add_test(pubsub_test pubsub_test)