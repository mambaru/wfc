#pragma once

#include <fas/aop.hpp>
#include <memory>
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{
  
struct ad_send_request
{
  mutable int tmp = 0;
  template<typename T, typename Params, typename Serializer>
  void operator()(
    T& t,
    const char* name,
    Params params, 
    Serializer ser,
    typename T::result_handler_t  result_handler
  ) const
  {
    std::shared_ptr<Params> p = nullptr;
    if (params!=nullptr)
    {
      p = std::make_shared<Params>( std::move(params) );
    }
    
    if ( tmp < 10000 )
    {
#warning DEBUG
      DAEMON_LOG_MESSAGE("DEBUG /handler/aspect/ad_send_request N " << tmp++);
    }
    
    t.send_request(
      name,
      std::move(result_handler), 
      [p, ser](const char* name, typename T::call_id_t id) 
        -> typename T::data_ptr
      {
        return ser(name, std::move(*p), id);
      }
    );
  }
};

}} // wfc


