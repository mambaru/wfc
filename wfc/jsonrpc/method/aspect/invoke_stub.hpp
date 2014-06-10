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
  
  template<typename T, typename TT>
  void operator()(T&, TT&, incoming_holder holder, ::wfc::io::outgoing_handler_t outgoing_handler) const
  {
    TT::template invoke_error<T, error_json>( std::move(holder), std::make_unique<method_not_impl>(), std::move(outgoing_handler) );
  }
};

}} // wfc


