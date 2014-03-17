#pragma once

#include <wfc/io/ip/tcp/rn/client_options.hpp>
#include <wfc/io/ip/tcp/rn/client.hpp>
#include <wfc/jsonrpc/service.hpp>

#include <wfc/memory.hpp>
#include <wfc/io_service.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
  
class client
{
public:
  typedef client_options options_type;
  ~client();
  client(io_service& io, const options_type& conf, std::shared_ptr<wfc::jsonrpc::service> service);
  void start();
  void stop();
  void shutdown();

private:
  std::shared_ptr<wfc::io::ip::tcp::rn::client> _client;
  std::shared_ptr<wfc::jsonrpc::service> _service;
  
};
  
}}}}}}
