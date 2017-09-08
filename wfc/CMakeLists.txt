include(BuildInfo)
include(Wfc)

SET(
  wfc_source_list
  wfc.cpp
  wfc_exit.cpp
  system/system.cpp
  core/global.cpp
  core/cpuset.cpp
  core/instance_args.cpp
  core/extended_args.cpp
  statistics/statistics.cpp
  module/component.cpp
  module/module.cpp
  module/package.cpp
  ../../iow/iow/logger/global_log.cpp
)

add_library(
  wfc
  ${wfc_source_list}
)

target_link_libraries(wfc iow ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

build_info(wfc wfc)

install(TARGETS wfc DESTINATION ${CMAKE_INSTALL_PREFIX})
