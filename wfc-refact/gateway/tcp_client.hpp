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

#include <wfc/gateway/tcp_options.hpp>
#include <wfc/gateway/multi_client.hpp>
#include <wfc/gateway/multi_client_options.hpp>

#include <wfc/io/ip/tcp/rn/jsonrpc/client.hpp>

/*
namespace wfc{ namespace jsonrpc{
class service;
}}
*/
/*
namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
class client;
}}}}}}
*/


namespace wfc{ namespace gateway{
  
class tcp_client
{
  typedef ::wfc::io::ip::tcp::rn::jsonrpc::client client_item_type;
  typedef ::wfc::asio::io_service io_service_type;
  typedef multi_client<client_item_type> client_type;
  typedef std::shared_ptr<client_type>  client_ptr;
  typedef std::list<client_ptr> list_type;
  typedef client_item_type::rpc_ptr rpc_ptr;
  typedef client_type::options_type options_type;
  
public:
  
  tcp_client( std::shared_ptr< ::wfc::global> g, const tcp_options& conf);
  
  tcp_client( io_service_type& io, const tcp_options& conf, rpc_ptr jsonrpc);
  
  void reconfigure(const tcp_options& conf);
  
  void initialize(rpc_ptr jsonrpc);
  
  void start();
  
  void stop();

private:
  
  void create( io_service_type& io, const tcp_options& conf);
  
private:
  
  std::shared_ptr< ::wfc::global> _global;
  
  io_service_type& _io_service;
  
  tcp_options _conf;
  
  rpc_ptr _jsonrpc;
  std::list< client_ptr > _tcp_clients;
};

}}
