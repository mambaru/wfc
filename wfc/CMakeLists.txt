
if ( WFC_READY )
  include(BuildInfo)
  include(Wfc)
  #set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  #include(Wfc)
  #link_directories( "${CMAKE_CURRENT_SOURCE_DIR}../iow/build/")
endif()

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

if ( WFC_READY )
  build_info(wfc wfc)
endif()

install(TARGETS wfc DESTINATION ${CMAKE_INSTALL_PREFIX})
