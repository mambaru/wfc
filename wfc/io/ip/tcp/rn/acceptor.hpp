#pragma once

#include <wfc/io/ip/tcp/rn/acceptor_options.hpp>
#include <wfc/memory.hpp>
#include <boost/asio.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class acceptor_impl;
  
class acceptor
{
public:
  typedef acceptor_options options_type;
  typedef boost::asio::ip::tcp::acceptor descriptor_type;
  ~acceptor();
  acceptor(descriptor_type&& desc, const options_type& conf, wfc::io::handler handler = nullptr);
  void start();
  void stop();
  void shutdown();
private:
  std::unique_ptr<acceptor_impl> _impl;
};
  
}}}}}
