#pragma once
#include <wfc/io/types.hpp>
#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
struct ad_callback_error
{
  template<typename T>
  void operator()( T& t, const error& err, incoming_holder holder, ::wfc::io::outgoing_handler_t callback)
  {
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(err);
    
    auto id_range = holder.raw_id();
    error_message.id = std::make_unique<typename T::data_type>( id_range.first, id_range.second );
    //error_message.id = std::move( holder.raw_id() );
    t.get_aspect().template get<_callback_json_>()(t, json_type(), std::move(error_message), holder.detach(), callback );
  }

  template<typename T>
  void operator()( T& t, const error& err, ::wfc::io::outgoing_handler_t callback)
  {
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(err);
    error_message.id = nullptr;
    t.get_aspect().template get<_callback_json_>()(t, json_type(), std::move(error_message), callback );
  }
};
  
}} // wfc
