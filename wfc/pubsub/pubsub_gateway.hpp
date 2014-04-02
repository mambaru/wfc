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
#include <wfc/pubsub/pubsub_gateway_options.hpp>

namespace wfc{ namespace pubsub{
  
class pubsub_gateway
  : public ::wfc::io::basic_io<>
  , public std::enable_shared_from_this< pubsub_gateway >
  , public ipubsub
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
  void stop();
  void process_outgoing( ::wfc::io::data_ptr d );
  
  ///
  /// interface
  ///
  virtual void describe( size_t subscriber_id );
  
  virtual void subscribe(request_subscribe_ptr, subscribe_callback, size_t subscriber_id, publish_handler );

  virtual void publish(request_publish_ptr, publish_callback);

  virtual void publish(request_multi_publish_ptr, multi_publish_callback);
  
  virtual void load(request_load_ptr, load_callback );

  virtual void load(request_multi_load_ptr, multi_load_callback );

  virtual void query(request_query_ptr, query_callback );
  
  virtual void notify(request_notify_ptr, notify_callback );

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



