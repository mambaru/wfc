#pragma once

#include <wfc/io/ip/tcp/rn/connection_base.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

class connection_impl
  : public connection_base<>
{
  typedef connection_base<> super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;

  connection_impl(descriptor_type&& desc, const options_type& conf, wfc::io::handler handler = nullptr)
    : super( std::move(desc), conf, handler)
  {
  }
  
  void stop(std::function<void()> finalize)
  {
    std::cout << "--------- connection_impl::stop ---------" << std::endl; 
    super::stop(finalize);
    std::cout << "^^^^^^^^^ connection_impl::stop ^^^^^^^^^" << std::endl; 
  }
};
  
}}}}}
