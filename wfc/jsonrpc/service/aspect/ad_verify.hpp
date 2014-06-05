#pragma once

#include <wfc/jsonrpc/service/aspect/tags.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>
#include <wfc/logger.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
struct ad_verify
{
  template<typename T>
  void operator()( T& t,  incoming_holder holder, io::io_id_t io_id, io::outgoing_handler_t handler)
  {
    if ( holder.is_valid() )
    {
      t.get_aspect().template get<_handler_switch_>()(t,  std::move(holder), io_id, handler );
    }
    else
    {
      t.get_aspect().template get<_callback_error_>()(t, invalid_request(), std::move(holder), handler );
    }
  }
};
  
}} // wfc

