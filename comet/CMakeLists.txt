include(BuildInfo)
SET(
  comet_source_list
  comet.cpp
  core/global.cpp
  logger/global_log.cpp
  pubsub/message_queue.cpp
  pubsub/topic_hub.cpp
)

add_library(
  comet SHARED
  ${comet_source_list}
)
build_info(comet comet)