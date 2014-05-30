#pragma once

#include <wfc/jsonrpc/service/aspect/tags.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>
#include <wfc/logger.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
struct ad_result_handler
{
  template<typename T>
  void operator()( T& t, incoming_holder holder)
  {
    if ( auto handler = t.registry().get_result_handler( holder.get_id<int>() ) )
    {
      handler( std::move(holder) );
    }
  }
};

  
}} // wfc


