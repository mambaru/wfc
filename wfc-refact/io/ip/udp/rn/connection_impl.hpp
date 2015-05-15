#pragma once

#include <wfc/io/ip/udp/rn/connection_base.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

class connection_impl
  : public connection_base<>
{
  typedef connection_base<> super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;

  connection_impl(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf)
  {
  }
};
  
}}}}}
