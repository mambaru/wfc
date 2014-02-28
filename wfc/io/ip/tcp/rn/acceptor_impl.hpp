#pragma once

#include <wfc/io/ip/tcp/rn/acceptor_base.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class acceptor_impl
  : public acceptor_base<>
{
  typedef acceptor_base<> super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;

  acceptor_impl(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf )
  {
  }
};
  
}}}}}
