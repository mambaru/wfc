#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
struct ad_invoke_error
{
  // TODO; вынести в T!!! (handler и аспект handler)
  // НО адвайс оставить T::send_error
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

}} // wfc


