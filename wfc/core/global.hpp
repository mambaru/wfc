//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/registry.hpp>
#include <wfc/core/fire_list.hpp>
#include <wfc/core/module_options.hpp>
#include <wfc/core/ibuild_info.hpp>
#include <iow/asio.hpp>

void wfc_abort(std::string message);

namespace wfc{

struct wfcglobal
{
  typedef std::shared_ptr<wfcglobal> ptr;

  std::string program_name;
  std::string instance_name;
  std::shared_ptr<ibuild_info> program_build_info;
  std::shared_ptr<ibuild_info> wfc_build_info;

  module_options options;

  typedef std::function<bool()> fire_handler;
  typedef fire_list< fire_handler > fakir;
  fakir idle;
  fakir after_start;
  fakir before_stop;
  fakir after_stop;
  bool enable_callback_check  = false;

  object_registry registry;

  typedef ::iow::asio::io_service io_service_type;
  io_service_type& io_service;

  static ptr static_global;

  wfcglobal( io_service_type& io_service)
    : io_service(io_service)
  {}
};

}
