#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/service/aspect/tags.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>
#include <wfc/logger.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
struct ad_incoming_parser
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d, io::io_id_t io_id, io::outgoing_handler_t outgoing_handler)
  {
    /*
    std::weak_ptr<handler_base> jsonrpc_handler;
    
    jsonrpc_handler = t.registry().get_jsonrpc_handler(io_id);
    
    if ( jsonrpc_handler.lock() == nullptr )
    {
      jsonrpc_handler = t.get_prototype();
    };
    */
    
    try
    {
      while (d != nullptr)
      {
        incoming_holder hold(std::move(d) );
        d = hold.tail();
        t.get_aspect().template get<_verify_>()( t,  std::move(hold), io_id,/*jsonrpc_handler,*/ outgoing_handler );
      }
    }
    catch(const json::json_error& er)
    {
      COMMON_LOG_WARNING( "jsonrpc::service parse error: " << er.what() )
      t.get_aspect().template get<_callback_error_>()(t, parse_error(), outgoing_handler );
    }
    catch(const std::exception& ex)
    {
      DAEMON_LOG_ERROR( "jsonrpc::service parse error: " << ex.what() )
      t.get_aspect().template get<_callback_error_>()(t, server_error(ex.what() ), outgoing_handler);
    }
    catch(...)
    {
      DAEMON_LOG_ERROR( "jsonrpc::service server error: " << "unhandler exception" );
      t.get_aspect().template get<_callback_error_>()(t, server_error(), outgoing_handler);
    }
  }
};

  
}} // wfc


