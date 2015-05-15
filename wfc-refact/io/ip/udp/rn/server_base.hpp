#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/udp/rn/server_aspect.hpp>
#include <wfc/io/server/server.hpp>
#include <wfc/asio.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{ 

template<typename A = fas::aspect<> >
class server_base:
  public wfc::io::server::server< typename fas::merge_aspect<A, server_aspect>::type >
{
  typedef wfc::io::server::server< typename fas::merge_aspect<A, server_aspect>::type > super;
  
public:
  
  typedef typename super::options_type options_type; 
  typedef ::wfc::asio::io_service io_service_type;
  
  
  server_base(io_service_type& io, const options_type& conf/*, wfc::io::incoming_handler handler = nullptr*/)
    : super( io, conf/*, handler*/)
  {
  }
};
  
}}}}}
