#pragma once

#include <wfc/io/ip/udp/rn/server_base.hpp>
#include <wfc/asio.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

class server_impl
  : public server_base<>
{
  typedef server_base<> super;
public:
  typedef typename super::options_type options_type; 
  typedef ::wfc::asio::io_service io_service_type;

  server_impl(io_service_type& io, const options_type& conf/*, wfc::io::incoming_handler handler = nullptr*/)
    : super( io, conf/*, handler*/)
  {
  }
};
  
}}}}}
