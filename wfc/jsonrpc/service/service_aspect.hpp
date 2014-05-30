#pragma once
#include <iostream>

#include <wfc/io/tags.hpp>
#include <wfc/io/io_base.hpp>
#include <wfc/io/strand.hpp>
#include <wfc/io_service.hpp>
#include <wfc/json/json.hpp>
#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>
#include <wfc/jsonrpc/options.hpp>


#include "../worker.hpp"


#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/service/aspect/aspect.hpp>

namespace wfc{ namespace jsonrpc{
  
  /*
struct ad_error_handler
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, ::wfc::io::outgoing_handler_t callback)
  {
    t.process_result(t, std::move(holder), callback);
  }
};

struct ad_result_handler
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, ::wfc::io::outgoing_handler_t callback)
  {
    t.process_result(t, std::move(holder), callback);
  }
};


struct ad_handler_switch
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, std::weak_ptr<handler_base> hb, ::wfc::io::outgoing_handler_t handler)
  {
    if ( holder.has_method() )
    {
      t.get_aspect().template get<_method_handler_>()(t, std::move(holder), hb, handler );
    }
    else if ( holder.has_result() )
    {
      t.get_aspect().template get<_result_handler_>()(t, std::move(holder), handler );
    }
    else if ( holder.has_error() )
    {
      t.get_aspect().template get<_error_handler_>()(t, std::move(holder), handler );
    }
  }
};



struct ad_verify
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, std::weak_ptr<handler_base> hb, ::wfc::io::outgoing_handler_t handler)
  {
    if ( holder.is_valid() )
    {
      t.get_aspect().template get<_handler_switch_>()(t, std::move(holder), hb, handler );
    }
    else
    {
      t.get_aspect().template get<_callback_error_>()(t, invalid_request(), std::move(holder), handler );
    }
  }
};

struct ad_incoming
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d, ::wfc::io::io_id_t io_id, ::wfc::io::outgoing_handler_t callback)
  {
    std::weak_ptr<handler_base> handler;
    
    handler = t._io_reg.get_method_handler(io_id);
    
    if ( handler.lock() == nullptr )
    {
      handler = t._handler_prototype;
    };
    
    try
    {
      while (d != nullptr)
      {
        ::wfc::jsonrpc::incoming_holder hold(std::move(d) );
        d = hold.tail();
        t.get_aspect().template get<_verify_>()( t, std::move(hold), handler, callback );
      }
    }
    catch(const json::json_error& er)
    {
      COMMON_LOG_WARNING( "jsonrpc::service parse error: " << er.what() )
      t.get_aspect().template get<_callback_error_>()(t, parse_error(), callback );
    }
    catch(const std::exception& ex)
    {
      DAEMON_LOG_ERROR( "jsonrpc::service parse error: " << ex.what() )
      t.get_aspect().template get<_callback_error_>()(t, server_error(ex.what() ), callback);
    }
    catch(...)
    {
      DAEMON_LOG_ERROR( "jsonrpc::service server error: " << "unhandler exception" );
      t.get_aspect().template get<_callback_error_>()(t, server_error(), callback);
    }
  }
};
*/

/// /////////////////////////////////////////



struct service_basic_aspect: fas::aspect<
  fas::advice<_input_, ad_input>,
  fas::advice<_incoming_parser_, ad_incoming_parser>,
  fas::advice<_verify_,ad_verify>,
  fas::advice<_handler_switch_,ad_handler_switch>,
  fas::advice<_callback_json_,  ad_callback_json>,
  fas::advice<_callback_error_, ad_callback_error>,
  fas::advice<_error_handler_,  ad_error_handler>,
  fas::advice<_method_handler_, ad_method_handler>,
  fas::advice<_result_handler_, ad_result_handler>
>{};

struct service_aspect: fas::aspect<
  service_basic_aspect::advice_list,
  fas::type< ::wfc::io::_options_type_, ::wfc::jsonrpc::options>,
  fas::type<_worker_type_, wfc::io::strand >
  //fas::type<_worker_type_, worker<> >
>{};

  
}} // wfc


