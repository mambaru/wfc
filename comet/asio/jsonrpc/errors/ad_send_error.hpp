#pragma once

#include <comet/asio/jsonrpc/errors/error.hpp>
#include <comet/asio/jsonrpc/errors/error_json.hpp>
#include <comet/asio/jsonrpc/objects/outgoing_error.hpp>
#include <comet/asio/jsonrpc/objects/outgoing_error_json.hpp>
#include <comet/asio/jsonrpc/tags.hpp>
#include <comet/memory.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct ad_send_error
{
  typedef outgoing_error<error> error_type;
//  typedef outgoing_error_json<typename error_json::type> error_json_type;
  struct error_json_type
    : outgoing_error_json<typename error_json::type >::type
  {};
  
  
  template<typename T>
  void operator()(T& t, std::unique_ptr<error> err, std::unique_ptr<int> id)
  {
    error_type errmsg;
    errmsg.error = std::move(err);
    errmsg.id = std::move(id);
  
    t.get_aspect().template get<_send_json_>()(t, error_json_type(), errmsg);
  }
};



}}}}
