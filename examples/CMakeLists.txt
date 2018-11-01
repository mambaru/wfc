include(BuildInfo)

add_executable(example01 example01.cpp)
target_link_libraries(example01 ${wfc_liblist})

add_executable(example02 example02.cpp)
target_link_libraries(example02 ${wfc_liblist})

add_executable(example03 example03.cpp)
target_link_libraries(example03 ${wfc_liblist})

add_executable(example04 example04.cpp)
target_link_libraries(example04 ${wfc_liblist})

add_executable(example05 example05.cpp)
target_link_libraries(example05 ${wfc_liblist})

add_executable(example06 example06.cpp)
target_link_libraries(example06 ${wfc_liblist})

add_subdirectory(example_package)
add_executable(example07 example07.cpp)
target_link_libraries(example07 example_package ${wfc_liblist})

build_info(example07 example07)
