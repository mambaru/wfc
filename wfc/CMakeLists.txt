
if ( WFC_READY )
  include(BuildInfo)
  include(Wfc)
else()
  find_package(Boost COMPONENTS system program_options filesystem date_time REQUIRED)
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
)

add_library(
  wfc
  ${wfc_source_list}
)

target_link_libraries(wfc ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

if ( WFC_READY )
  build_info(wfc wfc)
endif()

install(TARGETS wfc DESTINATION ${CMAKE_INSTALL_PREFIX})
