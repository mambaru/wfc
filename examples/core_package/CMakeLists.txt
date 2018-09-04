include(BuildInfo)

add_library(
  core_package 
  core_package.cpp 
  core/core_module.cpp
  core/core/core_domain.cpp
  core/core/core_singleton.cpp
  core/startup/startup_domain.cpp
  core/startup/startup_singleton.cpp
)

build_info(core_package core)
