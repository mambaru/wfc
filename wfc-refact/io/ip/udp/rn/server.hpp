#pragma once

#include <wfc/io/ip/udp/rn/server_options.hpp>
#include <wfc/memory.hpp>
#include <wfc/asio.hpp>
#include <boost/asio.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

class server_impl;
  
class server
{
public:
  typedef server_options options_type;
  typedef boost::asio::ip::udp::socket descriptor_type;
  typedef ::wfc::asio::io_service io_service_type;
  ~server();
  server(io_service_type& io, const options_type& conf);
  void start();
  void stop();
  void shutdown();
private:
  std::shared_ptr<server_impl> _impl;
};
  
}}}}}
