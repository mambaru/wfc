#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <memory>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class sequence_provider
  : public provider_base<Itf>
{
  typedef provider_base<Itf> super;
  typedef typename super::send_func send_func;
  
public:

  sequence_provider(const provider_config& conf)
    : super(conf)
  {
  }

  virtual void confirm() override
  {
    
  }
  
  virtual void rollback() override
  {
    
  }

  virtual void send(send_func /*fn*/) override
  {
    
  }
};

}}
