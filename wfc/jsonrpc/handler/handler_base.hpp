#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
class handler_base
{
public:
  typedef incoming_holder holder_type;
  typedef holder_type::data_type data_type;
  typedef holder_type::data_ptr  data_ptr;
  typedef ::wfc::io::outgoing_handler_t outgoing_handler_t;
  typedef ::wfc::io::io_id_t io_id_t;
  typedef int call_id_t;
  
 
  virtual ~handler_base() {}
  
  virtual std::shared_ptr<handler_base> clone( ) const= 0;
  
  virtual void process(holder_type holder, outgoing_handler_t outgoing_handler) = 0;
  
  virtual std::vector<std::string> get_methods() const = 0;
  
  virtual void start(io_id_t io_id) = 0;
  
  virtual void stop(io_id_t io_id) = 0;

  typedef std::function< data_ptr(const char* name, call_id_t id) > request_serializer_t;
  typedef std::function< data_ptr(const char* name) > notify_serializer_t;
  typedef std::function< void(incoming_holder) > result_handler_t;
  
  typedef std::function< void(const char* name, result_handler_t, request_serializer_t) > send_request_t;
  typedef std::function< void(const char* name, notify_serializer_t) > send_notify_t;
  
  send_request_t send_request = nullptr;
  send_notify_t  send_notify = nullptr;
  
};

}} // wfc


