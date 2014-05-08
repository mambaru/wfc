#pragma once
#include <iostream>

#include <wfc/io/tags.hpp>
#include <wfc/io/io_base.hpp>
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

namespace wfc{ namespace jsonrpc{
  

/// end OUTGOING JSON


struct _incoming_;

/// /////////////////////////////////////////

struct _send_json_;

struct ad_send_json
{
  
  template<typename T, typename J, typename O>
  void operator()( T&, J, O obj, /*incoming_holder holder*/ typename T::data_ptr d, ::wfc::io::outgoing_handler_t callback)
  {
    //auto d = holder.detach();
    d->clear();
    d->reserve(80);
    typename J::serializer()(obj, std::inserter( *d, d->end() ));
    //holder.handler( std::move(d) );
    callback( std::move(d) );
  }
  
  
  template<typename T, typename J, typename O>
  void operator()( T&, J, O obj, ::wfc::io::outgoing_handler_t handler)
  {
    auto d = std::make_unique< ::wfc::io::data_type>();
    d->reserve(80);
    typename J::serializer()(obj, std::inserter( *d, d->end() ));
    handler( std::move(d) );
  }
};


struct _jsonrpc_error_;
struct ad_jsonrpc_error
{
  template<typename T>
  void operator()( T& t, const error& err, incoming_holder holder, ::wfc::io::outgoing_handler_t callback)
  {
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(err);
    error_message.id = std::move( holder.raw_id() );
    t.get_aspect().template get<_send_json_>()(t, json_type(), std::move(error_message), /*std::move(holder)*/holder.detach(), callback );
  }

  template<typename T>
  void operator()( T& t, const error& err, /*wfc::io::callback handler,*/ ::wfc::io::outgoing_handler_t callback)
  {
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(err);
    error_message.id = nullptr;
    t.get_aspect().template get<_send_json_>()(t, json_type(), std::move(error_message), /*handler,*/ callback );
  }
};

struct _process_error_;
struct _process_method_;
struct _process_result_;


struct ad_process_method
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, std::weak_ptr<handler_base> hb, ::wfc::io::outgoing_handler_t callback)
  {
    t.push_advice(t, std::move( holder ), hb, callback );
  }
};

struct ad_process_error
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, ::wfc::io::outgoing_handler_t callback)
  {
    t.process_result(t, std::move(holder), callback);
  }
};

struct ad_process_result
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, ::wfc::io::outgoing_handler_t callback)
  {
    t.process_result(t, std::move(holder), callback);
  }
};

struct _process_;
struct ad_process
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, std::weak_ptr<handler_base> hb, ::wfc::io::outgoing_handler_t handler)
  {
    if ( holder.has_method() )
    {
      t.get_aspect().template get<_process_method_>()(t, std::move(holder), hb, handler );
    }
    else if ( holder.has_result() )
    {
      t.get_aspect().template get<_process_result_>()(t, std::move(holder), handler );
    }
    else if ( holder.has_error() )
    {
      t.get_aspect().template get<_process_error_>()(t, std::move(holder), handler );
    }
  }
};


struct _verify_;
struct ad_verify
{
  template<typename T>
  void operator()( T& t, incoming_holder holder, std::weak_ptr<handler_base> hb, ::wfc::io::outgoing_handler_t handler)
  {
    if ( holder.is_valid() )
    {
      t.get_aspect().template get<_process_>()(t, std::move(holder), hb, handler );
    }
    else
    {
      t.get_aspect().template get<_jsonrpc_error_>()(t, invalid_request(), std::move(holder), handler );
    }
  }
};

struct ad_incoming
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d, ::wfc::io::io_id_t id, ::wfc::io::outgoing_handler_t callback)
  {
    auto  handler = t._io_map.find(id)->second.method_handler;
    try
    {
      while (d != nullptr)
      {
        incoming_holder hold(std::move(d) );
        d = hold.tail();
        t.get_aspect().template get<_verify_>()( t, std::move(hold), handler, callback );
      }
    }
    catch(const json::json_error& er)
    {
      t.get_aspect().template get<_jsonrpc_error_>()(t, parse_error(), callback );
    }
    catch(const std::exception& ex)
    {
      // TODO; сделать логгирование 
      t.get_aspect().template get<_jsonrpc_error_>()(t, server_error(ex.what() ), callback);
    }
    catch(...)
    {
      // TODO; сделать логгирование 
      t.get_aspect().template get<_jsonrpc_error_>()(t, server_error(), callback);
    }
    
  }
};


/// /////////////////////////////////////////


struct _worker_type_;

struct service_basic_aspect: fas::aspect<
  fas::advice<_incoming_, ad_incoming>,
  fas::advice<_verify_,ad_verify>,
  fas::advice<_process_,ad_process>,
  fas::advice<_send_json_, ad_send_json>,
  fas::advice<_jsonrpc_error_, ad_jsonrpc_error>,
  fas::advice<_process_error_,  ad_process_error>,
  fas::advice<_process_method_, ad_process_method>,
  fas::advice<_process_result_, ad_process_result>
>{};

struct service_aspect: fas::aspect<
  service_basic_aspect::advice_list,
  fas::type< ::wfc::io::_options_type_, ::wfc::jsonrpc::options>,
  fas::type<_worker_type_, worker<> >
>{};

  
}} // wfc


