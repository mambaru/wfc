#pragma once

#include <comet/inet/imux.hpp>
#include <comet/core/registry.hpp>
#include <comet/core/imodule.hpp>
#include <comet/core/iconfig.hpp>
#include <comet/core/icore.hpp>
#include <comet/core/ilogger.hpp>

#include <memory>

namespace mamba{ namespace comet{

struct global
{ 
  typedef registry<imodule> module_registry;
  std::string program_name;
  std::string program_version;
  std::string comet_version;
  std::weak_ptr<icore>           core;
  std::weak_ptr<iconfig>         config;
  std::weak_ptr<ilogger>         logger;
  std::weak_ptr<module_registry> modules;
  std::weak_ptr< inet::imux<> >  mux;
  global()
  {}

  static std::weak_ptr<ilogger>  global_logger;
};

}}
