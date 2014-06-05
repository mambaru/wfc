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
  typedef handler_base::result_handler_t   result_handler_t;
  typedef std::map<int, result_handler_t>  result_map_t;

  std::shared_ptr<handler_base> jsonrpc_handler;
  
  io::outgoing_handler_t outgoing_handler;
  
  result_map_t result_map;

  io_info( std::shared_ptr<handler_base> jsonrpc_handler, io::outgoing_handler_t outgoing_handler)
    : jsonrpc_handler(jsonrpc_handler)
    , outgoing_handler(outgoing_handler)
  {}
};


class io_registry
{
  typedef io_info::result_handler_t result_handler_t;
  typedef std::map< io::io_id_t, io_info > io_map_t;
  typedef std::map< int, io::io_id_t     > call_io_map_t;
  
public:
  
  io_registry();
  
  void set_io(io::io_id_t io_id, std::shared_ptr<handler_base> jsonrpc_handler, io::outgoing_handler_t outgoing_handler);
  
  std::shared_ptr<handler_base> erase_io( io::io_id_t io_id );
  
  std::pair<int, io::outgoing_handler_t>
  add_result_handler(io::io_id_t io_id, result_handler_t result_handler);

  result_handler_t get_result_handler(int call_id) const;
  
  std::weak_ptr<handler_base> get_jsonrpc_handler(io::io_id_t io_id) const;
  
  io::outgoing_handler_t get_outgoing_handler(io::io_id_t io_id) const;
  
  void clear();
  
private:
  
  int _call_id_counter;
  io_map_t _io_map;
  call_io_map_t _call_io_map;
};

}} // wfc

