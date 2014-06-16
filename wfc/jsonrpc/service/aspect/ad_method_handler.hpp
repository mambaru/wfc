#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{
  
struct ad_method_handler
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, io::io_id_t io_id, ::wfc::io::outgoing_handler_t outgoing_handler)
  {
    std::weak_ptr<typename T::handler_interface> jsonrpc_handler;
    
    jsonrpc_handler = t.registry().get_jsonrpc_handler(io_id);
    
    if ( jsonrpc_handler.lock() == nullptr )
    {
      jsonrpc_handler = t.get_prototype();
    };
    
    if ( auto wrk = t.get_worker( holder.method() ).lock() )
    {
      auto ph = std::make_shared<incoming_holder>(std::move(holder) );
      wrk->post(  
        [&t, ph, jsonrpc_handler, outgoing_handler]()
        {
          if ( auto h = jsonrpc_handler.lock() )
          {
            h->invoke( std::move(*ph), outgoing_handler );
          }
          else
          {
            std::cerr << "jsonrpc logic error: jsonrpc handler error" << std::endl;
            DAEMON_LOG_FATAL("jsonrpc logic error: jsonrpc handler error" );
            abort();
          }
        }
      );
    }
    else
    {
      COMMON_LOG_WARNING("jsonrpc worker unavailable for method: " << holder.method() )
      t.get_aspect().template get<_callback_error_>()(t, service_unavailable(), std::move(holder), outgoing_handler );
    }
  }
};

}} // wfc::jsonrpc


