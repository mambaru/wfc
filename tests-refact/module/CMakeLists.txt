add_executable(registry_test registry_test.cpp)
add_executable(object_base_test  object_base_test.cpp)
message(STATUS "------->${Boost_FOUND}")
message(STATUS "------->${Boost_LIBRARIES}")
target_link_libraries(object_base_test ${Boost_LIBRARIES})


add_test(
  NAME registry_test 
  COMMAND registry_test 
  WORKING_DIRECTORY ${WFC_TEST_DIR} 
)

add_test(
  NAME object_base_test 
  COMMAND object_base_test 
  WORKING_DIRECTORY ${WFC_TEST_DIR} 
)