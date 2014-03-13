#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/handler/tags.hpp>
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
  
  //template<typename T>
  //void operator()(T&, incoming_holder) const
  template<typename T>
  void operator()(T& , incoming_holder , ::wfc::io::callback ) const
  {
    // TODO: send error
    abort();
  }
};

}} // wfc


