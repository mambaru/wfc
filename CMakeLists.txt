cmake_minimum_required(VERSION 3.1)

project(wfc CXX)

include(cmake/ci.cmake)
include(cmake/BuildInfo.cmake)
include(cmake/EnableStat.cmake)

add_subdirectory(config)
add_subdirectory(wfc)

wci_submodule(NAME faslib  )
wci_submodule(NAME wjson   )
wci_submodule(NAME wlog    )
wci_submodule(NAME wflow   )
wci_submodule(NAME iow     )
wci_submodule(NAME wjrpc   )
wci_submodule(NAME wrtstat )

target_include_directories(wfc PUBLIC "${wfc_SOURCE_DIR}")


if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  if ( WITH_SAMPLES )
    add_subdirectory(examples)
  endif()
endif()
