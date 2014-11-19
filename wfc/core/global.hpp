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
  
  typedef interface_registry registry_type;
  registry_type registry;

  ::wfc::io_service& io_service;

  static std::shared_ptr<global>   static_global;
  
  global(::wfc::io_service& io_service)
    : io_service(io_service)
  {
    
  }
};

}
