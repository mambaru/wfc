#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
template<size_t ReserveSize>
struct send_error
  : fas::type<_send_error_, send_error<ReserveSize> >
{
  template<typename T, typename ErrorJson>
  static inline void send(
    typename T::holder_type holder, 
    std::unique_ptr<typename ErrorJson::target> err, 
    typename T::outgoing_handler_t outgoing_handler
  )
  {
    typedef typename T::data_type data_type;
    typedef ErrorJson error_json;
    typedef typename ErrorJson::target error_type;

    typedef outgoing_error_json< error_json > message_json;
    outgoing_error<error_type> error_message;
    
    error_message.error = std::move(err);
    auto id_range = holder.raw_id();
    error_message.id = std::make_unique<data_type>( id_range.first, id_range.second );

    auto d = holder.detach();
    d->clear();
    d->reserve(ReserveSize);
    typename message_json::serializer()(error_message, std::inserter( *d, d->end() ));
    outgoing_handler( std::move(d) );
  }
};

struct send_error_proxy
  : fas::type<_send_error_, send_error_proxy >
{
  template<typename T, typename ErrorJson>
  static inline void send(
    typename T::holder_type holder, 
    std::unique_ptr<typename ErrorJson::target> err, 
    typename T::outgoing_handler_t outgoing_handler
  )
  {
    return T::aspect::template advice_cast<_send_error_>::type
            ::template send<T, ErrorJson>( std::move(holder), std::move(err), std::move(outgoing_handler) );
  }
};


}} // wfc


