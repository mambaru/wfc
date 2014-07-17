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
  ~client();
  client( ::wfc::io_service& io, const options_type& conf);
  wfc::io::io_id_t get_id() const;
  void start();
  void stop(std::function<void()> finalize);
  void shutdown();
private:
  std::shared_ptr<client_impl> _impl;
};
  
}}}}}

