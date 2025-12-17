
update_ogenc(TARGETS wfc WARNINGS -Wnull-dereference OFF)

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

wci_targets(wfc)

enable_stat(wfc)

target_compile_definitions(wfc PRIVATE -DBOOST_ASIO_NO_DEPRECATED)

build_info(TARGET wfc)

get_boost(system asio program_options filesystem date_time regex)

target_include_directories(wfc PUBLIC $<BUILD_INTERFACE:${wfc_SOURCE_DIR}> )

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
    PUBLIC
      Boost::system
      Boost::asio
      Boost::program_options
      Boost::filesystem
      Boost::date_time
      Boost::regex
)
