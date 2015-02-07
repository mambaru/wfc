#pragma once

#include <wfc/io/ip/tcp/rn/acceptor_options.hpp>
#include <wfc/memory.hpp>
#include <wfc/asio.hpp>
#include <boost/asio.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class acceptor_impl;
  
class acceptor
{
public:
  typedef acceptor_options options_type;
  typedef boost::asio::ip::tcp::acceptor descriptor_type;
  typedef wfc::asio::io_service io_service_type;
  
  ~acceptor();
  acceptor(descriptor_type&& desc, const options_type& conf);
  void reconfigure(const options_type& conf);
  void listen();
  void start();
  void close();
  void stop(std::function<void()> finalize);
  void shutdown();
  
  std::shared_ptr<acceptor> clone( io_service_type& io);
  
private:
  acceptor(std::shared_ptr<acceptor_impl> impl);
  
  std::shared_ptr<acceptor_impl> _impl;
};
  
}}}}}
