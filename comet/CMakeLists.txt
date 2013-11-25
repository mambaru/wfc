include(BuildInfo)
SET(
  comet_source_list
  comet.cpp
  core/global.cpp
)

add_library(
  comet SHARED
  ${comet_source_list}
)
build_info(comet comet)