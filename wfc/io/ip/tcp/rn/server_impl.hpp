#pragma once

#include <wfc/io/ip/tcp/rn/server_base.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class server_impl
  : public server_base<>
{
  typedef server_base<> super;
public:
  typedef typename super::options_type options_type; 

  server_impl(wfc::io_service& io, const options_type& conf)
    : super( io, conf )
  {
  }
};
  
}}}}}
