add_executable(incoming_holder incoming_holder.cpp)
target_link_libraries(incoming_holder wfc boost_system pthread)

add_executable(invoke invoke.cpp)
target_link_libraries(invoke wfc boost_system pthread)


add_executable(call call.cpp)
target_link_libraries(call wfc boost_system pthread)
