//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/global.hpp>
#include <wfc/io_service.hpp>
#include <wfc/io/io_base.hpp>
#include <wfc/jsonrpc/service.hpp>

namespace wfc{ namespace pubsub{
  
struct pubsub_gateway_options: 
  ::wfc::io::basic::options
{
  std::string pubsub_name;
  
  std::string incoming_target;
  std::string incoming_channel;
  
  std::string outgoing_target;
  std::string outgoing_channel;
};

class pubsub_gateway
  : ::wfc::io::basic_io<>
{
public:
  typedef ::wfc::io::basic_io<> super;
  typedef pubsub_gateway_options options_type;
  typedef ::wfc::jsonrpc::service jsonrpc_service;
  typedef std::shared_ptr<jsonrpc_service> jsonrpc_ptr;
  typedef std::weak_ptr< ::wfc::pubsub::ipubsub > ipubsub_ptr;
  
  virtual ~pubsub_gateway();
  pubsub_gateway( std::weak_ptr< ::wfc::global > global, const options_type& conf);
  void initialize( std::shared_ptr< ::wfc::jsonrpc::service> jsonrpc);
  void start();
  void process_outgoing( ::wfc::io::data_ptr d );
private:
  void process_outgoing_( ::wfc::io::data_ptr d );
private:
  std::weak_ptr< ::wfc::global > _global;
  options_type _options; // вынести в аспект basic_io
  jsonrpc_ptr  _jsonrpc;
  ::wfc::io::io_id_t _io_id;
  ipubsub_ptr _incoming_target;
  ipubsub_ptr _outgoing_target;
};

}}



