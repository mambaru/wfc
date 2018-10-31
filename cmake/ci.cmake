# copy this file to root project: cp external/cmake-ci/ci.cmake cmake/
# include(cmake/ci.cmake)


MACRO(get_libs liblist)

  if ( NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}" )
    return()
  endif()

  message(STATUS "${PROJECT_NAME} init submodules")
  execute_process(COMMAND git submodule update --init WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  message(STATUS "${PROJECT_NAME} init submodules DONE")
  foreach(curlib liblist)
    message(curlib)
  endforeach(loop_var)

ENDMACRO(get_libs)
