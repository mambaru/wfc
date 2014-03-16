#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
class handler_base
{
public:
  typedef ::wfc::io::data_type data_type;
  typedef ::wfc::io::data_ptr  data_ptr;
  
  typedef std::function< ::wfc::io::data_ptr(const char* name, int id)   > request_serializer_t;
  typedef std::function< void(incoming_holder holder)  > incoming_handler_t;
  typedef std::function< void(const char* name, incoming_handler_t, request_serializer_t) > outgoing_request_handler_t;

  
  virtual ~handler_base() {}
  virtual std::shared_ptr<handler_base> clone( /*outgoing_request_handler_t request_handler*/ ) const= 0;
  virtual void process(incoming_holder holder, ::wfc::io::callback callback) = 0;
  
  virtual void start(::wfc::io::io_id_t id) = 0;
  virtual void stop(::wfc::io::io_id_t id) = 0;
  // ???
  std::function< void( ::wfc::io::data_ptr /*d*/, ::wfc::io::data_ptr /*id*/, std::shared_ptr<handler_base>) > outgoing = nullptr;
  
  
  outgoing_request_handler_t outgoing_request_handler = nullptr;
};

}} // wfc


