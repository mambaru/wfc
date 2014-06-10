#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_result_json.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_result.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
struct ad_invoke_result
{
  // TODO; вынести в T!!! (handler и аспект handler)
  // Не как статик (чтоб можно было подменить в методе)
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

}} // wfc


