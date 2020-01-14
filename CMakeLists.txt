cmake_minimum_required(VERSION 3.1)

project(wfc)

#if ( WFC_ENABLE_STAT OR BUILD_TESTING)
#  add_definitions(-DWFC_ENABLE_STAT)
#endif()

#TODO OR BUILD_TESTING
#if ( WFC_ENABLE_STAT )
#  wci_add_options(-DWFC_ENABLE_STAT)
#endif()

include(cmake/ci.cmake)
include(cmake/BuildInfo.cmake)
add_subdirectory(config)

add_subdirectory(wfc)
target_include_directories(wfc PUBLIC "${wfc_SOURCE_DIR}")

wci_getlib(NAME faslib  )
wci_getlib(NAME wjson   )
wci_getlib(NAME wlog    )
wci_getlib(NAME wflow   )
wci_getlib(NAME iow     )
wci_getlib(NAME wjrpc   )
wci_getlib(NAME wrtstat )

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  if ( WITH_SAMPLES )
    add_subdirectory(examples)
  endif()
endif()




