#pragma once

#include <wfc/io/ip/tcp/rn/connection_options.hpp>
#include <wfc/memory.hpp>
#include <wfc/io_service.hpp>
#include <boost/asio.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class connection_impl;
  
class connection
{
public:
  typedef connection_options options_type;
  typedef boost::asio::ip::tcp::socket descriptor_type;
  ~connection();
  connection(descriptor_type&& desc, const options_type& conf, wfc::io::handler handler = nullptr);
  wfc::io::io_id_t get_id() const;
  void start();
  void stop();
  void shutdown();
  
  const ::wfc::io_service::strand& strand() const;
  ::wfc::io_service::strand& strand();
private:
  std::unique_ptr<connection_impl> _impl;
};
  
}}}}}
