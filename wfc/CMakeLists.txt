message(STATUS "=========2============> ${IOW_DIR}")
message(STATUS "=========3============> ${IOW_LIB}")

include(BuildInfo)
include(mambaopt)

find_package(Boost COMPONENTS system program_options filesystem date_time REQUIRED)

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

target_link_libraries(wfc ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${WLOG_LIB})

build_info(wfc wfc)

install(TARGETS wfc DESTINATION ${CMAKE_INSTALL_PREFIX})
