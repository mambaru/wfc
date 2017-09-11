add_executable(registry_test registry_test.cpp)
add_executable(domain_test  domain_test.cpp)
add_executable(instance_test  instance_test.cpp)
add_executable(object_test  object_test.cpp)
add_executable(module_test  module_test.cpp)

target_link_libraries(domain_test wfc iow ${Boost_LIBRARIES})
target_link_libraries(instance_test wfc iow ${Boost_LIBRARIES})
target_link_libraries(object_test wfc iow ${Boost_LIBRARIES})
target_link_libraries(module_test wfc iow ${Boost_LIBRARIES})
target_link_libraries(registry_test wfc iow ${Boost_LIBRARIES})


add_test(
  NAME registry_test 
  COMMAND registry_test 
  WORKING_DIRECTORY ${WFC_TEST_DIR} 
)

add_test(
  NAME domain_test 
  COMMAND domain_test 
  WORKING_DIRECTORY ${WFC_TEST_DIR} 
)

add_test(
  NAME instance_test 
  COMMAND instance_test
  WORKING_DIRECTORY ${WFC_TEST_DIR} 
)

add_test(
  NAME object_test 
  COMMAND object_test
  WORKING_DIRECTORY ${WFC_TEST_DIR} 
)

add_test(
  NAME module_test 
  COMMAND module_test
  WORKING_DIRECTORY ${WFC_TEST_DIR} 
)
