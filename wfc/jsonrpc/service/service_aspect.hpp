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
  void operator()( T&, J, O obj, incoming_holder holder)
  {
    auto d = holder.detach();
    d->clear();
    d->reserve(80);
    typename J::serializer()(obj, std::inserter( *d, d->end() ));
    holder.handler( std::move(d) );
  }
  
  template<typename T, typename J, typename O>
  void operator()( T&, J, O obj, wfc::io::callback handler)
  {
    auto d = std::make_unique<wfc::io::data_type>();
    d->reserve(80);
    typename J::serializer()(obj, std::inserter( *d, d->end() ));
    handler( std::move(d) );
  }
};


struct _jsonrpc_error_;
struct ad_jsonrpc_error
{
  template<typename T>
  void operator()( T& t, const error& err, incoming_holder holder)
  {
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(err);
    error_message.id = std::move( holder.raw_id() );
    t.get_aspect().template get<_send_json_>()(t, json_type(), std::move(error_message), std::move(holder) );
  }

  template<typename T>
  void operator()( T& t, const error& err, wfc::io::callback handler)
  {
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(err);
    error_message.id = nullptr;
    t.get_aspect().template get<_send_json_>()(t, json_type(), std::move(error_message), handler );
  }
};

struct _process_error_;
struct _process_method_;
struct _process_result_;


struct ad_process_method
{
  template<typename T>
  void operator()( T& t, incoming_holder holder)
  {
    // t.tmp_worker->operator()( std::move(holder) );
    t.push_advice(t, std::move( holder ) );
  }
};

struct ad_process_error
{
  template<typename T>
  void operator()( T&, incoming_holder)
  {
    
  }
};

struct ad_process_result
{
  template<typename T>
  void operator()( T&, incoming_holder)
  {
  }
};

struct _process_;
struct ad_process
{
  template<typename T>
  void operator()( T& t, incoming_holder holder)
  {
    if ( holder.has_method() )
    {
      t.get_aspect().template get<_process_method_>()(t, std::move(holder) );
    }
    else if ( holder.has_result() )
    {
      t.get_aspect().template get<_process_result_>()(t, std::move(holder) );
    }
    else if ( holder.has_error() )
    {
      t.get_aspect().template get<_process_error_>()(t, std::move(holder) );
    }
  }
};


struct _verify_;
struct ad_verify
{
  template<typename T>
  void operator()( T& t, incoming_holder holder)
  {
    if ( holder.is_valid() )
    {
      t.get_aspect().template get<_process_>()(t, std::move(holder) );
    }
    else
    {
      t.get_aspect().template get<_jsonrpc_error_>()(t, invalid_request(), std::move(holder) );
    }
  }
};

struct ad_incoming
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d, std::weak_ptr<wfc::io::iio> iio, wfc::io::callback handler)
  {
    std::cout << "ad_incoming { " << std::string( d->begin(), d->end()) << std::endl;
    try
    {
      while (d != nullptr)
      {
        std::cout << "ad_incoming next: " << std::string( d->begin(), d->end()) << std::endl;
        incoming_holder hold(std::move(d), iio, handler );
        d = hold.tail();
        t.get_aspect().template get<_verify_>()( t, std::move(hold) );
      }
    }
    catch(const json::json_error& er)
    {
      t.get_aspect().template get<_jsonrpc_error_>()(t, parse_error(), handler );
    }
    catch(const std::exception& ex)
    {
      // TODO; сделать логгирование 
      t.get_aspect().template get<_jsonrpc_error_>()(t, server_error(ex.what() ), handler );
    }
    catch(...)
    {
      // TODO; сделать логгирование 
      t.get_aspect().template get<_jsonrpc_error_>()(t, server_error(), handler );
    }
    std::cout << "} ad_incoming" << std::endl;
  }
};


/// /////////////////////////////////////////

struct service_options
  : worker_options
{
  struct service
  {
    struct queue
    {
      int count = 1;
      std::vector<std::string> methods = {"*"};
    };
    
    int threads = 0;
    std::vector<queue> queues = {queue()};
  };
  std::vector< service> services = { service() };
};

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
  fas::type<wfc::io::_options_type_, service_options>,
  fas::type<_worker_type_, worker<> >
>{};

  
}} // wfc


