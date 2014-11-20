#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/gateway/rn/jsonrpc/gateway_config.hpp>
#include <wfc/io_service.hpp>
#include <wfc/core/global.hpp>
#include <wfc/pubsub/pubsub_gateway.hpp>
#include <list>

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

#include <wfc/gateway/tcp_options.hpp>

namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
class client;
}}}}}}


namespace wfc{ namespace gateway{
  
class tcp_client
{
  typedef ::wfc::io::ip::tcp::rn::jsonrpc::client client_item_type;
  typedef client_list<client_item_type> client_type;
  typedef std::shared_ptr<client_type>  client_ptr;
  typedef std::list<client_ptr> list_type;
  
  typedef client_item_type::options_type client_item_options;
  typedef client_options<client_item_options> options_type;
  
  /*
  typedef ::wfc::jsonrpc::service jsonrpc_type;
  typedef ::wfc::io::ip::tcp::rn::jsonrpc::client client_tcp_type;
  typedef std::shared_ptr<client_tcp_type> client_tcp_ptr;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;
  */
  
public:
  
  tcp_client( std::weak_ptr< ::wfc::global> g, const tcp_options& conf);
  
  tcp_client( ::wfc::io_service& io, const tcp_options& conf, jsonrpc_ptr jsonrpc);
  
  void reconfigure(const tcp_options& conf);
  
  void initialize(jsonrpc_ptr jsonrpc);
  
  void start();
  
  void stop();

private:
  
  void create( ::wfc::io_service& io, const tcp_options& conf);
  
private:
  
  std::weak_ptr< ::wfc::global> _global;
  
  ::wfc::io_service& _io_service;
  
  tcp_options _conf;
  
  jsonrpc_ptr _jsonrpc;
  std::list< client_tcp_ptr > _tcp_clients;
};

}}
