#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace jsonrpc{
  
struct ad_error_handler
{
  template<typename T>
  void operator()( T& t, incoming_holder holder)
  {
    t.get_aspect().template get< _result_handler_ >()(t, std::move(holder) );
  }
};

  
}} // wfc


