#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/callback/callback_status.hpp>
#include <memory>
#include <functional>

namespace wfc{ namespace jsonrpc{
  
template<
  typename Req, 
  typename Resp, 
  typename I, 
  void (I::*mem_ptr)( std::unique_ptr<Req>, std::function< void(std::unique_ptr<Req>) > ) 
>
struct mem_fun_handler
{
  
  template<typename T>
  void operator()(T& t, std::unique_ptr<Req> req, std::function< void(std::unique_ptr<Resp>, std::unique_ptr<wfc::jsonrpc::error>) > callback)
  {
    if ( auto i = t.target().lock() )
    {
      if ( callback==nullptr)
      {
        (i.get()->*mem_ptr)( std::move(req), nullptr );
      }
      else
      {
        (i.get()->*mem_ptr)( std::move(req), [callback](std::unique_ptr<Resp> resp)
        {
          callback( std::move(resp), nullptr);
        });
      }
    }
  }
};

}} // wfc


