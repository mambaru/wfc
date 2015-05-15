#pragma once

#include <wfc/io/ip/tcp/rn/client_base.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class client_impl
  : public client_base<>
{
  typedef client_base<> super;
public:
  typedef typename super::options_type options_type; 
  typedef ::wfc::asio::io_service io_service_type;

  client_impl( io_service_type& io, const options_type& conf)
    : super( io, conf)
  {
  }
};
  
}}}}}
