include(BuildInfo)
SET(
  wfc_source_list
  wfc.cpp
  core/global.cpp
  logger/global_log.cpp
  pubsub/message_queue.cpp
  pubsub/topic_hub.cpp
)

add_library(
  wfc SHARED
  ${wfc_source_list}
)

target_link_libraries(wfc boost_system)

build_info(wfc wfc)