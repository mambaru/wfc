#pragma once

#include <wfc/io/ip/tcp/rn/client_options.hpp>
#include <wfc/io/ip/tcp/rn/client.hpp>
#include <wfc/jsonrpc/service.hpp>

#include <wfc/memory.hpp>
#include <wfc/asio.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace jsonrpc{
class service;
}}

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{
  
class client
{
public:
  typedef client_options options_type;
  typedef ::wfc::jsonrpc::service rpc_type;
  typedef ::wfc::asio::io_service io_service_type;
  
  typedef std::shared_ptr< rpc_type > rpc_ptr;
  ~client();
  client(io_service_type& io, const options_type& conf, rpc_ptr service);
  void start();
  void stop(std::function<void()> finalize);
  void shutdown();

private:
  std::shared_ptr< ::wfc::io::ip::tcp::rn::client > _client;
  std::shared_ptr< ::wfc::jsonrpc::service > _service;
  
};
  
}}}}}}
