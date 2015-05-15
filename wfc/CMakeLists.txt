include(BuildInfo)
include(Wfc)

find_package(Boost COMPONENTS system REQUIRED)
# CONFIGURE_LIBRARY(boost/system/error_code.hpp  "/usr/include /usr/local/include"
#                  boost_system-mt   "/lib /usr/lib /usr/local/lib /usr/lib64")

include_directories(..)
include_directories(../../faslib/)
include_directories(../../iow/)

SET(
  wfc_source_list
  
)

add_library(
  wfc SHARED
  ${wfc_source_list}
)

target_link_libraries(wfc ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )
#target_link_libraries(wfc ${HAVE_boost_system-mt} ${CMAKE_THREAD_LIBS_INIT} )

build_info(wfc wfc)

install(TARGETS wfc DESTINATION ${CMAKE_INSTALL_PREFIX})
