#pragma once

#include <wfc/jsonrpc/service/service_aspect.hpp>
#include <wfc/jsonrpc/service/io_registry.hpp>
#include <wfc/io/strand.hpp>

#include <thread>
#include <list>
#include <map>
#include <memory>

namespace wfc{ namespace jsonrpc{

class worker_manager
{
  typedef ::wfc::io::strand worker_type;
  typedef std::shared_ptr< worker_type > worker_ptr;
  
  typedef std::list<worker_ptr> worker_list;
  typedef std::pair<worker_list, typename worker_list::iterator > pair_worker_list;
  typedef std::map< std::string, pair_worker_list> method_map;
  typedef std::list< std::thread > thread_list;
  typedef std::shared_ptr< ::wfc::io_service> io_service_ptr;
  typedef std::list<io_service_ptr> service_list; 

public:
  
  typedef options options_type;
  
  worker_manager(wfc::io_service& io_service, const options_type& config);
  
  void start();
  
  void stop();
  
  std::weak_ptr< worker_type > get_worker(const char* name);

private:
  wfc::io_service& _io_service;
  options_type _config;

  method_map _method_map;
  worker_list _workers;
  service_list _services;
  thread_list _threads;
};


}} // wfc


