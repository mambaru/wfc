#pragma once

#include <wfc/jsonrpc/method/mem_fun/mem_fun_helper.hpp>

namespace wfc{ namespace jsonrpc{

template<
  typename Interface,
  typename Target, 
  void (Target::*mem_ptr)( io::io_id_t, std::weak_ptr<Interface> ) 
>
struct mem_fun_startup
{
  template<typename T>
  void operator()(T& t, io::io_id_t id) const
  {
    if (auto trg = t.provider().lock() )
    {
      (trg.get()->*mem_ptr)( id, t.shared_from_this() );
    }
    else
    {
    }
  }
};

}} // wfc


