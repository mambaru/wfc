#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider/sequence_provider.hpp>
#include <wfc/gateway/provider/simple_provider.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <memory>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class provider
  //: public iprovider<Itf>
{
public:
  
  typedef provider_base<Itf> abstract_type;
  typedef std::shared_ptr<abstract_type> abstract_ptr;
  
  typedef sequence_provider<Itf> sequence_type;
  typedef simple_provider<Itf>   simple_type;
  
  typedef Itf interface_type;
  
  provider(const provider_config& conf)
  {
    if ( conf.sequence_mode  )
    {
      _impl = std::make_shared<sequence_type>(conf);
    }
    else
    {
      _impl = std::make_shared<simple_type>(conf);
    }
  }
  
  void startup(size_t clinet_id, std::shared_ptr<interface_type> ptr ) 
  {
    _impl->startup(clinet_id, ptr );
  }

  void shutdown(size_t clinet_id) 
  {
    _impl->shutdown(clinet_id);
  }

  
private:
  
  abstract_ptr _impl;
};

}}
