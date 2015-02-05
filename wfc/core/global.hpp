//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/registry.hpp>
#include <wfc/core/fire_list.hpp>
#include <wfc/io_service.hpp>
#include <wfc/core/module_options.hpp>

namespace wfc{
  
struct global
{ 
  std::string program_name;
  std::string program_version;
  std::string wfc_version;
  std::string instance_name;
  module_options options;

  typedef std::function<void()> fire_handler;
  typedef fire_list< fire_handler > fakir;
  fakir idle;
  fakir after_start;
  fakir before_stop;
  fakir after_stop;
  

  typedef interface_registry registry_type;
  registry_type registry;

  ::wfc::io_service& io_service;

  static std::shared_ptr<global>   static_global;
  
  global(::wfc::io_service& io_service)
    : io_service(io_service)
  {}
  
};

}
