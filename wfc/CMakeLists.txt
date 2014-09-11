include(BuildInfo)
include(Wfc)

IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.7)
  find_package(Boost COMPONENTS system thread program_options REQUIRED)
ELSE (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.7)
  CONFIGURE_LIBRARY(boost/system/error_code.hpp "/usr/include /usr/local/include"
                    boost_system-mt "/lib /usr/lib /usr/local/lib /usr/lib64")
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.7)   

include_directories(..)
include_directories(../faslib/)

SET(
  wfc_source_list
  wfc.cpp
  thread/delayed_queue.cpp
  core/global.cpp
  logger/global_log.cpp
  pubsub/message_queue.cpp
  pubsub/topic_hub.cpp
  pubsub/pubsub_gateway.cpp
  jsonrpc/incoming/incoming_holder.cpp
  jsonrpc/service.cpp
  jsonrpc/service/io_registry.cpp
  jsonrpc/service/worker_manager.cpp
  #io/posix/rn/reader.cpp
  #io/posix/rn/writer.cpp
  io/ip/tcp/rn/connection.cpp
  io/ip/tcp/rn/server.cpp
  io/ip/tcp/rn/acceptor.cpp
  io/ip/tcp/rn/client.cpp
  io/ip/tcp/rn/jsonrpc/server.cpp
  io/ip/tcp/rn/jsonrpc/client.cpp

  io/ip/udp/rn/connection.cpp
  io/ip/udp/rn/acceptor.cpp
  io/ip/udp/rn/server.cpp
  io/ip/udp/rn/jsonrpc/server.cpp

  service/rn/jsonrpc/service.cpp
  service/rn/jsonrpc/service_list.cpp
  gateway/rn/jsonrpc/gateway.cpp
  gateway/rn/jsonrpc/gateway_list.cpp

  system/detail/aligned_allocator.cpp
)

add_library(
  wfc SHARED
  ${wfc_source_list}
)

IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.7)
  target_link_libraries(wfc ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )
ELSE (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.7)
  target_link_libraries(wfc ${HAVE_boost_system-mt} ${CMAKE_THREAD_LIBS_INIT} )
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.7) 

build_info(wfc wfc)

install(TARGETS wfc DESTINATION ${CMAKE_INSTALL_PREFIX})
