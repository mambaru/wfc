include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if ( NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}" )
  message(STATUS "${PROJECT_NAME} is not top level project")
  return()
endif()

include(mambaopt)
include(mambalibs)

#get_faslib()
#get_mambaru(wjson WJSON_DIR "" "")
#get_mambaru(wlog WLOG_DIR WLOG_LIB "")
#get_mambaru(wflow WFLOW_DIR WFLOW_LIB "")
#get_mambaru(iow IOW_DIR IOW_LIB "")
#get_mambaru(wjrpc WJRPC_DIR WJRPC_LIB "")
#get_mambaru(wrtstat WRTSTAT_DIR WRTSTAT_LIB "")
