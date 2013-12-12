#pragma once

//#include <comet/inet/imux.hpp>
#include <comet/core/registry.hpp>
#include <comet/module/imodule.hpp>
#include <comet/core/iconfig.hpp>
#include <comet/core/icore.hpp>
#include <comet/logger/ilogger.hpp>
#include <comet/callback/callback_list.hpp>
#include <comet/pubsub/ipubsub.hpp>

#include <boost/asio/io_service.hpp>
#include <memory>
#include <chrono>
#include <functional>

namespace mamba{ namespace comet{

struct global
{ 
  typedef registry<imodule> module_registry;
  typedef registry<ilogger> logger_registry;
  
  typedef std::function<callback_status()> idle_callback;
  callback_list<idle_callback> idle;
  
  
  std::string program_name;
  std::string program_version;
  std::string comet_version;
  std::string instance_name;
  std::weak_ptr<icore>           core;
  std::weak_ptr<iconfig>         config;
  // std::weak_ptr<ilogger>         logger;
  std::weak_ptr<logger_registry> loggers;
  std::weak_ptr<module_registry> modules;
  
  std::weak_ptr<boost::asio::io_service> io_service;
  //std::weak_ptr<inet::imux<>>  mux;
  
  //std::chrono::milliseconds
  //void  idle(std::chrono::milliseconds ms, std::function<void()> handle);

  static std::weak_ptr<global>   static_global;
  /*
    static std::weak_ptr<ilogger>  global_logger;
    static std::weak_ptr<icore>    global_core;
  */
};

}}
