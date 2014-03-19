include(BuildInfo)
include(Wfc)

CONFIGURE_LIBRARY(boost/system/error_code.hpp  "/usr/include /usr/local/include"
                  boost_system-mt   "/lib /usr/lib /usr/local/lib /usr/lib64")

SET(
  wfc_source_list
  wfc.cpp
  thread/delayed_queue.cpp
  core/global.cpp
  logger/global_log.cpp
  pubsub/message_queue.cpp
  pubsub/topic_hub.cpp
  pubsub/pubsub_gateway.cpp
  io/posix/rn/reader.cpp
  io/posix/rn/writer.cpp
  io/ip/tcp/rn/connection.cpp
  io/ip/tcp/rn/server.cpp
  io/ip/tcp/rn/acceptor.cpp
  io/ip/tcp/rn/client.cpp
  io/ip/tcp/rn/jsonrpc/server.cpp
  io/ip/tcp/rn/jsonrpc/client.cpp
  service/rn/jsonrpc/service.cpp
  service/rn/jsonrpc/service_list.cpp
  gateway/rn/jsonrpc/gateway.cpp
  gateway/rn/jsonrpc/gateway_list.cpp
)

add_library(
  wfc SHARED
  ${wfc_source_list}
)

target_link_libraries(wfc ${HAVE_boost_system-mt} ${CMAKE_THREAD_LIBS_INIT} )

build_info(wfc wfc)