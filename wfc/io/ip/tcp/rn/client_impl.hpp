#pragma once

#include <wfc/io/ip/tcp/rn/client_base.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class client_impl
  : public client_base<>
{
  typedef client_base<> super;
public:
  typedef typename super::options_type options_type; 
  //typedef typename super::descriptor_type descriptor_type;

  client_impl(wfc::io_service& io, const options_type& conf/*, wfc::io::handler handler = nullptr*/)
    : super( io, conf/*, handler*/)
  {
  }
};
  
}}}}}
