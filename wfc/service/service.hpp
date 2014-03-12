#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/core/global.hpp>
#include <list>

namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
class server;
}}}}}}


namespace wfc{

struct service_config
{
  
};

class service
{
  typedef wfc::jsonrpc::service jsonrpc_type;
  typedef wfc::io::ip::tcp::rn::jsonrpc::server server_tcp_type;
  typedef std::shared_ptr<server_tcp_type> server_tcp_ptr;
  typedef std::shared_ptr<jsonrpc_type> jsonrpc_ptr;
public:
  service(std::weak_ptr< wfc::global > g, const service_config& conf, const wfc::jsonrpc::handler_base& init);
  
  void reconfigure(const service_config& conf);
  
  void initialize( );
  
  void start();
  
  void stop();

private:
  jsonrpc_ptr _jsonrpc;
  std::list< server_tcp_ptr > _tcp_servers;
};

}
