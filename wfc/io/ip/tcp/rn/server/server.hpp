#pragma once

#include "aspect.hpp"
#include "config.hpp"
#include <wfc/io/server/server.hpp>
#include <wfc/io_service.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace impl{ 
  
class server
  : public ::wfc::io::server::server< aspect1 >
{
public:  
  typedef ::wfc::io::server::server< aspect1 > super;

  server( wfc::io_service& io)
    : super(io)
  {
    
  }
  
  server( wfc::io_service& io,  const config& conf)
    : super(io, conf)
  {
    
  }

};
  
}}}}}}