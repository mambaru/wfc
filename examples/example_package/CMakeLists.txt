include(BuildInfo)

include_directories(.)
add_library(
  example_package 
  example_package.cpp 

  core/core_module.cpp
  core/core/core_domain.cpp
  core/core/core_singleton.cpp
  core/startup/startup_domain.cpp
  core/startup/startup_singleton.cpp

  example/example_module.cpp
  example/domain/example_domain.cpp
  example/domain/example_multiton.cpp
)

build_info(example_package example)
