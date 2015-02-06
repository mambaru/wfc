add_executable(registry_test registry_test.cpp)

add_test(NAME registry_test COMMAND registry_test WORKING_DIRECTORY ${WFC_TEST_DIR} )