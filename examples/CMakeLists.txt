add_definitions(-DBOOST_ASIO_NO_DEPRECATED)

include(BuildInfo)

add_executable(example01 example01.cpp)
target_link_libraries(example01 PUBLIC wfc)

add_executable(example02 example02.cpp)
target_link_libraries(example02 PUBLIC wfc)

add_executable(example03 example03.cpp)
target_link_libraries(example03 PUBLIC wfc)

add_executable(example04 example04.cpp)
target_link_libraries(example04 PUBLIC wfc)

add_executable(example05 example05.cpp)
target_link_libraries(example05 PUBLIC wfc)

add_executable(example06 example06.cpp)
target_link_libraries(example06 PUBLIC wfc)

add_subdirectory(example_package)
add_executable(example07 example07.cpp)
target_link_libraries(example07 PUBLIC example_package wfc)

build_info(example07 example07)
wci_targets(example01 example02 example03 example04 example05 example06 example07 )
