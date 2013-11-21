SET(
  comet_source_list
  comet.cpp
)

add_library(
  comet SHARED
  ${comet_source_list}
)
