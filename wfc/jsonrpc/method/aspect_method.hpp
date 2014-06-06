#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/aspect/call.hpp>
#include <wfc/jsonrpc/method/aspect/invoke.hpp>
#include <wfc/jsonrpc/method/aspect/invoke_stub.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
struct _invoke_error_;
struct ad_invoke_error
{
  template<typename T, typename J>
  static void send_error(typename T::holder_type holder, std::unique_ptr<typename J::target> err, typename T::outgoing_handler_t outgoing_handler)
  {
    typedef J error_json;
    typedef typename J::target error_type;

    typedef outgoing_error_json< error_json > message_json;
    outgoing_error<error_type> error_message;
    
    error_message.error = std::move(err);
    auto id_range = holder.raw_id();
    error_message.id = std::make_unique<typename T::data_type>( id_range.first, id_range.second );

    auto d = holder.detach();
    d->clear();
    typename message_json::serializer()(error_message, std::inserter( *d, d->end() ));
    outgoing_handler( std::move(d) );
  }
};

struct _invoke_result_;
struct ad_invoke_result
{
  template<typename T, typename J>
  static void send_result(typename T::holder_type holder, std::unique_ptr<typename J::target> result, typename T::outgoing_handler_t outgoing_handler)
  {
    typedef J result_json;
    typedef typename J::target result_type;
    
    outgoing_result<result_type> result_message;
    result_message.result = std::move(result);
              
    auto id_range = holder.raw_id();
    result_message.id = std::make_unique<typename T::data_type>( id_range.first, id_range.second );

    auto d = holder.detach();
    d->clear();
    typedef outgoing_result_json<result_json> message_json;
    typename message_json::serializer()(result_message, std::inserter( *d, d->end() ));
    outgoing_handler( std::move(d) );
  }
};



struct aspect_method
  : fas::aspect< 
      fas::advice<_invoke_error_, ad_invoke_error>,
      fas::advice<_invoke_result_, ad_invoke_result>,
      invoke_stub
    > 
{
  
};

}} // wfc


