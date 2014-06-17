#pragma once

#include <wfc/jsonrpc/service/aspect/tags.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
// TODO: rename holder_switch
struct ad_handler_switch
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, io::io_id_t io_id,/*std::weak_ptr<handler_base> jsonrpc_handler,*/ io::outgoing_handler_t outgoing_handler)
  {
    if ( holder.has_method() )
    {
      t.get_aspect().template get<_method_handler_>()(t, std::move(holder), io_id, /*jsonrpc_handler,*/ outgoing_handler );
    }
    else if ( holder.has_result() )
    {
      t.get_aspect().template get<_result_handler_>()(t, std::move(holder));
    }
    else if ( holder.has_error() )
    {
      t.get_aspect().template get<_error_handler_>()(t, std::move(holder));
    }
  }
};

  
}} // wfc


