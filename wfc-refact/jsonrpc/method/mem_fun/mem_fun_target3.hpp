#pragma once

#include <wfc/jsonrpc/method/mem_fun/mem_fun_helper.hpp>

namespace wfc{ namespace jsonrpc{

template<
  typename Target,
  void (Target::*mem_ptr)( io::io_id_t )
>
struct mem_fun_target3
{
  template<typename T>
  void operator()(T& t, io::io_id_t id) const
  {
    if ( auto i = t.target() )
    {
      (i.get()->*mem_ptr)( id );
    }
    else
    {
    }
  }
};

}} // wfc


