//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/registry.hpp>
#include <wfc/core/fire_list.hpp>
#include <wfc/core/module_options.hpp>
#include <iow/asio.hpp>

namespace wfc{
  
struct wfcglobal
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

  interface_registry registry;

  typedef ::iow::asio::io_service io_service_type;
  io_service_type& io_service;

  static std::shared_ptr<wfcglobal>   static_global;
  
  wfcglobal( io_service_type& io_service)
    : io_service(io_service)
  {}
};

}
