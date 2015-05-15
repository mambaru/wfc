#pragma once

#include <wfc/io/ip/udp/rn/acceptor_base.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

  /*
class acceptor_impl
  : public connection
{
  typedef connection super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;

  acceptor_impl(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf)
  {
  }
};
*/

class acceptor_impl
  : public acceptor_base<>
{
  typedef acceptor_base<> super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;

  acceptor_impl(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf)
  {
  }
};

  
}}}}}
