//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/registry.hpp>
#include <wfc/core/fire_list.hpp>
#include <wfc/core/extended_args.hpp>
#include <wfc/core/ibuild_info.hpp>
#include <wfc/core/workflow.hpp>
#include <wfc/core/thread_manager.hpp>
#include <wfc/core/cpuset.hpp>
#include <iow/asio.hpp>


namespace wfc{

struct wfcglobal
{
  typedef std::shared_ptr<wfcglobal> ptr;

  std::string program_name;
  std::string instance_name;
  std::shared_ptr<ibuild_info> program_build_info;
  std::shared_ptr<ibuild_info> wfc_build_info;

  extended_args args;

  typedef std::function<bool()> fire_handler;
  typedef fire_list< fire_handler > fakir;
  fakir idle;
  fakir after_start;
  fakir before_stop;
  fakir after_stop;
  bool enable_callback_check  = false;
  thread_manager threads;
  cpuset cpu;
  object_registry registry;

  typedef ::iow::asio::io_service io_service_type;
  io_service_type& io_service;
  std::shared_ptr< ::wfc::workflow > workflow;
  
  static ptr static_global;

  wfcglobal( io_service_type& io_service);
};

}
