#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/tcp/rn/server_aspect.hpp>
#include <wfc/io/server/server.hpp>
#include <wfc/asio.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 

template<typename A = fas::aspect<> >
class server_base:
  public wfc::io::server::server< typename fas::merge_aspect<A, server_aspect>::type >
{
  typedef wfc::io::server::server< typename fas::merge_aspect<A, server_aspect>::type > super;
  
public:
  
  typedef wfc::asio::io_service io_service;
  typedef typename super::options_type options_type; 
  
  
  server_base( io_service& io, const options_type& conf)
    : super( io, conf)
  {
  }
};
  
}}}}}
