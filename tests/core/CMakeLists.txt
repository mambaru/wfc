add_executable(test_core test_core.cpp vars_suite.cpp)
target_link_libraries(test_core PUBLIC fas::testing wfc)
wci_test(NAME test_core TARGET test_core)
