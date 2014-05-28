#pragma once

#include <wfc/jsonrpc/handler.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{
  
struct ad_method_handler
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, std::weak_ptr<handler_base> hb, ::wfc::io::outgoing_handler_t callback)
  {
    if ( auto wrk = t.get_worker( holder.method() ) )
    {
      auto ph = std::make_shared<incoming_holder>(std::move(holder) );
      wrk->post(  
        [&t, ph, hb, callback]()
        {
          if ( auto h = hb.lock() )
          {
            h->process( std::move(*ph), callback );
          }
          else
          {
            std::cerr << "jsonrpc logic error: jsonrpc handler error" << std::endl;
            DAEMON_LOG_FATAL("jsonrpc logic error: jsonrpc handler error" );
            abort();
            /*
            std::cout << "jsonrpc handler unavailable. Method: " << ph->method() << std::endl;
            DAEMON_LOG_WARNING("jsonrpc handler unavailable. Method: " << ph->method() )
            t.get_aspect().template get<_callback_error_>()(t, service_unavailable(), std::move(*ph), callback );
            */
          }
        }
      );
    }
    else
    {
      COMMON_LOG_WARNING("jsonrpc worker unavailable for method: " << holder.method() )
      t.get_aspect().template get<_callback_error_>()(t, service_unavailable(), std::move(holder), callback );
    }
  }
};

}} // wfc::jsonrpc


