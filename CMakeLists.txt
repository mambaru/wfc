cmake_minimum_required(VERSION 3.1)

project(wfc)

if ( WFC_ENABLE_STAT OR BUILD_TESTING)
  add_definitions(-DWFC_ENABLE_STAT)
endif()

include(cmake/ci.cmake)
add_subdirectory(config)

add_subdirectory(wfc)
target_include_directories(wfc PUBLIC "${wfc_SOURCE_DIR}")

wci_getlib(NAME faslib  ${WFC_TOPLEVEL})
wci_getlib(NAME wjson   ${WFC_TOPLEVEL})
wci_getlib(NAME wlog    ${WFC_TOPLEVEL})
wci_getlib(NAME wflow   ${WFC_TOPLEVEL}) 
wci_getlib(NAME iow     ${WFC_TOPLEVEL}) 
wci_getlib(NAME wjrpc   ${WFC_TOPLEVEL}) 
wci_getlib(NAME wrtstat ${WFC_TOPLEVEL}) 

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
  if ( WITH_SAMPLES )
    add_subdirectory(examples)
  endif()
endif()




