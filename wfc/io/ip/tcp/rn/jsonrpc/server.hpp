#pragma once

#include <wfc/io/ip/tcp/rn/server_options.hpp>
#include <wfc/io/ip/tcp/rn/server.hpp>
#include <wfc/jsonrpc/service.hpp>

#include <wfc/memory.hpp>
#include <wfc/asio.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
  
class server
{
public:
  typedef server_options options_type;
  typedef ::wfc::asio::io_service io_service_type;
  ~server();
  server(io_service_type& io, const options_type& conf, std::shared_ptr<wfc::jsonrpc::service> service);
  void start();
  void stop();
  void shutdown();

private:
  std::shared_ptr<wfc::io::ip::tcp::rn::server> _server;
  std::shared_ptr<wfc::jsonrpc::service> _service;
  
};
  
}}}}}}
