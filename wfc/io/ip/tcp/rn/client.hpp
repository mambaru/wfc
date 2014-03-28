#pragma once

#include <wfc/io/ip/tcp/rn/client_options.hpp>
#include <wfc/memory.hpp>
#include <wfc/io_service.hpp>
#include <boost/asio.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class client_impl;
  
class client
{
public:
  typedef client_options options_type;
  //typedef boost::asio::ip::tcp::socket descriptor_type;
  ~client();
  client( ::wfc::io_service& io, const options_type& conf, wfc::io::handler handler = nullptr);
  wfc::io::io_id_t get_id() const;
  void start();
  void stop();
  void shutdown();
private:
  std::unique_ptr<client_impl> _impl;
};
  
}}}}}
