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

add_library(wfc ${wfc_source_list})
wci_targets(wfc)
build_info(wfc wfc)

include(FindThreads)
find_package(Boost COMPONENTS system program_options filesystem date_time regex REQUIRED)

target_link_libraries(
  wfc
    PUBLIC
      iow
      wflow
      wjrpc
      wrtstat
      wlog
      wjson
      faslib
      ${Boost_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
)


