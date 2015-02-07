#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/gateway/rn/jsonrpc/gateway_config.hpp>
#include <wfc/asio.hpp>
#include <wfc/core/global.hpp>
#include <wfc/pubsub/pubsub_gateway.hpp>
#include <list>

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

#include <wfc/gateway/gateway_config.hpp>

namespace wfc{ namespace jsonrpc{
class service;
}}


namespace wfc{ namespace gateway{

class tcp_client;

class gateway
{
  typedef ::wfc::jsonrpc::service jsonrpc_type;
  typedef std::shared_ptr<tcp_client>   tcp_client_ptr;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;
  
public:
  
  typedef ::wfc::asio::io_service io_service_type;
  
  gateway( std::weak_ptr< ::wfc::global> g, const gateway_config& conf);
  
  gateway( io_service_type& io, const gateway_config& conf, jsonrpc_ptr jsonrpc);
  
  void reconfigure(const gateway_config& conf);
  
  void initialize(jsonrpc_ptr jsonrpc);
  
  void start();
  
  void stop();

private:
  
  void create( io_service_type& io, const gateway_config& conf);
  
private:
  
  std::weak_ptr< ::wfc::global> _global;
  
  io_service_type& _io_service;
  
  gateway_config _conf;
  jsonrpc_ptr _jsonrpc;
  std::list< tcp_client_ptr > _tcp_clients;
};

}}
