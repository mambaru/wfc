//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/object_registry.hpp>
#include <wfc/core/fire_list.hpp>
#include <wfc/core/extended_args.hpp>
#include <wfc/core/ibuild_info.hpp>
#include <wfc/core/workflow.hpp>
#include <wfc/core/cpuset.hpp>
#include <iow/asio.hpp>
#include <atomic>

namespace wfc{

struct wfcglobal
{
  typedef std::shared_ptr<wfcglobal> ptr;
  typedef std::function<bool()> fire_handler;
  typedef fire_list< fire_handler > fakir;
  typedef ::iow::asio::io_service io_service_type;

  std::string program_name;
  std::string instance_name;
  std::shared_ptr<ibuild_info> program_build_info;
  std::shared_ptr<ibuild_info> wfc_build_info;

  extended_args args;

  fakir idle;
  fakir after_start;
  fakir before_stop;
  fakir after_stop;
  
  std::atomic<bool> disable_statistics;
  std::atomic<bool> nocall_callback_abort;
  std::atomic<bool> nocall_callback_show;
  std::atomic<bool> double_callback_abort;
  std::atomic<bool> double_callback_show;
  
  cpuset cpu;
  object_registry registry;
  
  io_service_type& io_service;
  std::shared_ptr< workflow > common_workflow;
  static ptr static_global;
  std::atomic<bool> stop_signal_flag;

  explicit wfcglobal( io_service_type& io_service);
};

}
