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
    auto handler = t.registry().get_jsonrpc_handler(io_id);
    if ( handler == nullptr)
    {
      // Если обработчик не был создан create_handler
      // который вызываеться при новом коннекте 
      // то при каждом запросе создаем временный 
      // При этом обработка идет минуя воркеры
      // !!! handler = t.clone_prototype();
      handler = t.get_prototype();
    }
    
    if ( auto wrk = t.get_worker( holder.method() ) )
    {
      auto ph = std::make_shared<incoming_holder>(std::move(holder) );
      wrk->post(  
        [&t, io_id, ph, handler, outgoing_handler]()
        {
          // Проверяем, что пока дошла очередь, клиент не отвалился
          if ( ph->is_notify() || t.registry().has_io(io_id) )
          {
            handler->invoke( std::move(*ph), outgoing_handler );
          }
          else
          {
            COMMON_LOG_WARNING("wfc::jsonrpc::ad_method_handler: connection was reset when request queued");
          }
        }
      );
    }
    else if ( t.options().workers.empty() )
    {
      handler->invoke( std::move(holder), outgoing_handler ); 
    }
    else
    {
      COMMON_LOG_WARNING("jsonrpc worker unavailable for method: " << holder.method() )
      t.get_aspect().template get<_callback_error_>()(t, service_unavailable(), std::move(holder), outgoing_handler );
    }
  }
};

}} // wfc::jsonrpc
