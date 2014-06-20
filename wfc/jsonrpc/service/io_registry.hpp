#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <wfc/jsonrpc/service/service_aspect.hpp>

#include <wfc/io/io_base.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace jsonrpc{

struct io_info
{
  typedef ihandler handler_interface;
  typedef handler_interface::result_handler_t   result_handler_t;
  typedef handler_interface::io_id_t io_id_t;
  typedef handler_interface::call_id_t call_id_t;
  typedef handler_interface::outgoing_handler_t outgoing_handler_t;
  
  typedef std::map<call_id_t, result_handler_t>  result_map_t;

  std::shared_ptr<handler_interface> jsonrpc_handler;
  outgoing_handler_t outgoing_handler;
  
  
  result_map_t result_map;

  io_info( std::shared_ptr<handler_interface> jsonrpc_handler, outgoing_handler_t outgoing_handler)
    : jsonrpc_handler(jsonrpc_handler)
    , outgoing_handler(outgoing_handler)
  {}
};


class io_registry
{
  typedef io_info::handler_interface handler_interface;
  typedef io_info::result_handler_t result_handler_t;
  typedef io_info::io_id_t io_id_t;
  typedef io_info::call_id_t call_id_t;
  typedef io_info::outgoing_handler_t outgoing_handler_t;
  
  
  typedef std::map< io_id_t, io_info >   io_map_t;
  typedef std::map< call_id_t, io_id_t > call_io_map_t;
  
public:
  
  io_registry();
  
  void set_io(io_id_t io_id, std::shared_ptr<handler_interface> jsonrpc_handler, outgoing_handler_t outgoing_handler);
  
  std::shared_ptr<handler_interface> erase_io( io_id_t io_id );
  
  std::pair<call_id_t, outgoing_handler_t>
  add_result_handler(io_id_t io_id, result_handler_t result_handler);

  result_handler_t get_result_handler(call_id_t call_id) const;
  
  std::weak_ptr<handler_interface> get_jsonrpc_handler(io_id_t io_id) const;
  
  outgoing_handler_t get_outgoing_handler(io_id_t io_id) const;
  
  void clear();
  
private:
  
  call_id_t _call_id_counter;
  io_map_t _io_map;
  call_io_map_t _call_io_map;
};

}} // wfc

