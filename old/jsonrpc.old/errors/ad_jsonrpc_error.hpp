#pragma once

#include <wfc/jsonrpc/errors/error.hpp>
#include <wfc/jsonrpc/errors/tags.hpp>
#include <string>

namespace wfc{ namespace jsonrpc{

// TODO: убрать и сделать функциями, см. error.hpp bad_gateway
  
struct ad_parse_error
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_jsonrpc_error_>()(t, -32700, "Parse error");
  }
};

struct ad_invalid_request
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_jsonrpc_error_>()(t, -32600, "Invalid Request");
  }
};

struct ad_method_not_found
{
  template<typename T>
  void operator()(T& t, int id)
  {
    t.get_aspect().template get<_jsonrpc_error_>()(t, -32601, "Method not found",  id);
  }
};

struct ad_invalid_params
{
  template<typename T>
  void operator()(T& t, int id)
  {
    t.get_aspect().template get<_jsonrpc_error_>()(t, -32602, "Invalid params", id);
  }
};

struct ad_internal_error
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_jsonrpc_error_>()(t, -32603, "Internal error");
  }
};

struct ad_invalid_id
{
  template<typename T>
  void operator()(T& t, int id)
  {
    t.get_aspect().template get<_jsonrpc_error_>()(t, -32604, "Invalid id", id);
  }
};

struct ad_method_not_impl
{
  template<typename T>
  void operator()(T& t, int id)
  {
    t.get_aspect().template get<_jsonrpc_error_>()(t, -32605, "Method not implementation", id);
  }
};

struct ad_server_error
{
  template<typename T>
  void operator()(T& t, int code, std::string message)
  {
    t.get_aspect().template get<_send_error_>()
    (t, std::make_unique<error>(error({code,message})), nullptr);
  }

  template<typename T>
  void operator()(T& t, int code, std::string message,  int id)
  {
    t.get_aspect().template get<_send_error_>()
    (t, std::make_unique<error>(error({code,message})), std::make_unique<int>(id) );
  }

};

}}
