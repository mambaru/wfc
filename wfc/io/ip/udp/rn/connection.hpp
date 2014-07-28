#pragma once

#include <wfc/io/ip/udp/rn/acceptor_options.hpp>
#include <wfc/memory.hpp>
#include <wfc/io_service.hpp>
#include <boost/asio.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

class connection_impl;
  
class connection
{
public:
  typedef acceptor_options options_type;
  typedef boost::asio::ip::udp::socket descriptor_type;
  ~connection();
  connection(descriptor_type&& desc, const options_type& conf);
  void reconfigure( const options_type& conf );
  wfc::io::io_id_t get_id() const;
  void start();
  void listen();
  void close();
  void stop(std::function<void()> finalize);
  void shutdown();
  
  std::shared_ptr<connection> clone( ::wfc::io_service& io );
  
  ::wfc::io_service& get_io_service();
  const ::wfc::io_service::strand& strand() const;
  ::wfc::io_service::strand& strand();
  
  
private:
  std::shared_ptr<connection_impl> _impl;
  
  connection( std::shared_ptr<connection_impl> impl);
};
  
}}}}}
