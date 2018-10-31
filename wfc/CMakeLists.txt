include(BuildInfo)
include(mambaopt)

include(FindThreads)
find_package(Boost COMPONENTS system program_options filesystem date_time regex REQUIRED)


SET(
  wfc_source_list
  wfc.cpp
  wfc_exit.cpp
  iinterface.cpp
  winterface.cpp
  system/system.cpp
  core/wfcglobal.cpp
  core/cpuset.cpp
  core/instance_args.cpp
  core/extended_args.cpp
  core/object_registry.cpp
  statistics/statistics.cpp
  module/component.cpp
  module/module.cpp
  module/package.cpp
)

add_library(
  wfc
  ${wfc_source_list}
)

build_info(wfc wfc)

install(TARGETS wfc DESTINATION ${CMAKE_INSTALL_PREFIX})

if ( STANDALONE )
  set(
    liblst
    wrtstat
    wlog
    wflow
    wjrpc
    ${Boost_LIBRARIES} 
    ${CMAKE_THREAD_LIBS_INIT} 
  )
  target_link_libraries(wfc ${liblst})
endif()
