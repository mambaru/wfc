#add_library(
#  wfc_core_vars
#  OBJECT
#    core/vars.cpp
#    core/detail/private_vars.cpp
#)

#update_ogenc(TARGETS wfc_core_vars WARNINGS -Wnull-dereference OFF)

#target_link_libraries(wfc_core_vars PRIVATE iow wjson Boost::regex)

add_library(wfc
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
  core/vars.cpp
  core/detail/private_vars.cpp
  statistics/statistics.cpp
  module/component.cpp
  module/module.cpp
  module/package.cpp
  testing/testing_domain.cpp
  testing/testing_wfc.cpp
)

#wci_targets(wfc wfc_core_vars)
wci_targets(wfc)
enable_stat(wfc)

target_compile_definitions(wfc PRIVATE -DBOOST_ASIO_NO_DEPRECATED)
build_info(TARGET wfc)

get_boost(system program_options filesystem date_time regex)


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
#      wfc_core_vars
)

#add_dependencies(wfc wfc_core_vars)
