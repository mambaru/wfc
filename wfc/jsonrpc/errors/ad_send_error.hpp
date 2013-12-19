#pragma once

#include <wfc/jsonrpc/errors/error.hpp>
#include <wfc/jsonrpc/errors/error_json.hpp>
#include <wfc/jsonrpc/objects/outgoing_error.hpp>
#include <wfc/jsonrpc/objects/outgoing_error_json.hpp>
#include <wfc/jsonrpc/tags.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{

struct ad_send_error
{
  typedef outgoing_error<error> error_type;

  struct error_json_type
    : outgoing_error_json<typename error_json::type >::type
  {};
  
  template<typename T>
  void operator()(T& t, std::unique_ptr<error> err, std::unique_ptr<int> id)
  {
    error_type errmsg;
    errmsg.error = std::move(err);
    errmsg.id = std::move(id);
  
    //sleep(1);
    t.get_aspect().template get<_send_json_>()(t, error_json_type(), errmsg);
    t.close();
  }
};



}}
