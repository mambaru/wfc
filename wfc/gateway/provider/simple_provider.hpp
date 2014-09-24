#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <memory>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class simple_provider
  : public basic_provider<Itf, simple_provider>
  
{
  typedef basic_provider<Itf, simple_provider> super;
  typedef typename super::interface_type interface_type;
  
public:

  simple_provider(const provider_config& conf)
    : super(conf)
  {
  }
  
  template<typename Req, typename Callback, typename... Args>
  void post( 
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    if ( auto cli = super::get() )
    {
      (cli.get()->*mem_ptr)( std::move(req), std::move(callback), std::forward<Args>(args)... );
    }
  }

};

}}
