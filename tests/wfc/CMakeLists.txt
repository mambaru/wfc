add_executable(wfc_test wfc_test.cpp)
#link_directories(../../build/wfc)
target_link_libraries(wfc_test wfc )
add_test(wfc_test wfc_test)