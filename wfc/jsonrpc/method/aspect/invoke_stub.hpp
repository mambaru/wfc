#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
struct invoke_stub
{
  typedef fas::metalist::advice metatype;
  typedef _invoke_ tag;
  typedef invoke_stub advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  template<typename T>
  void operator()(T& , incoming_holder ph, ::wfc::io::outgoing_handler_t handler) const
  {
    // В аспект!
    typedef outgoing_error_json< error_json::type >::type json_type;
    outgoing_error<error> error_message;
    error_message.error = std::make_unique<error>(method_not_impl());
    error_message.id = std::move( ph.raw_id() );
              
    auto d = std::make_unique< ::wfc::io::data_type>();
    typename json_type::serializer()(error_message, std::inserter( *d, d->end() ));
    handler( std::move(d) );
  }
};

}} // wfc


