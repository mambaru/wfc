include(BuildInfo)
include(mambaopt)

include(FindThreads)
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

set(
  liblst
  ${WLOG_LIB}
  ${WFLOW_LIB}
  ${Boost_LIBRARIES} 
  ${CMAKE_THREAD_LIBS_INIT} 
)

if (WFC_ENABLE_STAT)
  set(
    liblst
    ${liblst}
    ${WRTSTAT_LIB}
  )
endif()

target_link_libraries(wfc ${liblst})

build_info(wfc wfc)

install(TARGETS wfc DESTINATION ${CMAKE_INSTALL_PREFIX})
