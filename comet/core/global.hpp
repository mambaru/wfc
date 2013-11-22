#pragma once

#include <comet/inet/imux.hpp>
#include <comet/core/registry.hpp>
#include <memory>

namespace mamba{ namespace comet{

class imodule;
class iconfig;
class icore;

struct global
{
  /*
  bool   daemonize;
  bool   coredump;
  bool   autoup;
  time_t autoup_timeout;
  std::string config_path;
  */
  
  std::weak_ptr<icore>   core;
  std::weak_ptr<iconfig> config;
  
  registry<imodule> modules;
  std::weak_ptr< inet::imux<> > mux;

  global()
    /*
    : daemonize(false)
    , coredump(false)
    , autoup(false)
    , autoup_timeout(0)
    */
  {}
};

}}
