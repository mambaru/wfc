#pragma once

//#include <wfc/inet/imux.hpp>
#include <wfc/core/registry.hpp>
#include <wfc/module/imodule.hpp>
#include <wfc/core/iconfig.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/fire_list.hpp>
#include <wfc/logger/ilogger.hpp>
//#include <wfc/callback/callback_list.hpp>
#include <wfc/pubsub/ipubsub.hpp>

#include <wfc/io_service.hpp>
#include <memory>
#include <chrono>
#include <functional>
#include <list>


#include <wfc/core/module_options.hpp>
namespace wfc{
  
struct global
{ 
  typedef registry<imodule> module_registry;
  typedef registry<ilogger> logger_registry;
  typedef registry<pubsub::ipubsub> pubsub_registry;
  typedef std::function<void()> idle_handler;
  fire_list< idle_handler > idle;

  std::string program_name;
  std::string program_version;
  std::string wfc_version;
  std::string instance_name;
  module_options options;
  
  std::shared_ptr< istartup >        startup;
  std::shared_ptr< icore >           core;
  std::shared_ptr< iconfig >         config;
  std::shared_ptr< logger_registry > loggers;
  std::shared_ptr< module_registry > modules;
  ::wfc::io_service& io_service;
  
  std::shared_ptr< pubsub_registry > pubsubs;

  static std::shared_ptr<global>   static_global;
  
  global(::wfc::io_service& io_service)
    : io_service(io_service)
  {
    
  }
};

}
