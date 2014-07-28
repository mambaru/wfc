#pragma once

#include <wfc/io/ip/udp/rn/server_options.hpp>
#include <wfc/io/ip/udp/rn/server.hpp>
#include <wfc/jsonrpc/service.hpp>

#include <wfc/memory.hpp>
#include <wfc/io_service.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{ namespace jsonrpc{
  
class server
{
public:
  typedef server_options options_type;
  ~server();
  server(io_service& io, const options_type& conf, std::shared_ptr<wfc::jsonrpc::service> service);
  void start();
  void stop();
  void shutdown();

private:
  std::shared_ptr<wfc::io::ip::udp::rn::server> _server;
  std::shared_ptr<wfc::jsonrpc::service> _service;
  
};
  
}}}}}}
