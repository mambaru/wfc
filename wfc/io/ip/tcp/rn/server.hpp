#pragma once

#include <wfc/io/ip/tcp/rn/server_options.hpp>
#include <wfc/memory.hpp>
#include <wfc/io_service.hpp>
#include <boost/asio.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class server_impl;
  
class server
{
public:
  typedef server_options options_type;
  typedef boost::asio::ip::tcp::socket descriptor_type;
  ~server();
  server(io_service& io, const options_type& conf/*, wfc::io::incoming_handler handler = nullptr*/);
  void start();
  void stop();
  void shutdown();
private:
  std::unique_ptr<server_impl> _impl;
};
  
}}}}}
