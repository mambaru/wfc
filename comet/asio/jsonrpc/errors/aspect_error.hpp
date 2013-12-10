#pragma once

#include <comet/asio/jsonrpc/errors/ad_send_error.hpp>
#include <comet/asio/jsonrpc/errors/error.hpp>
#include <comet/asio/jsonrpc/errors/tags.hpp>
#include <comet/memory.hpp>
#include <fas/aop.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

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

struct ad_not_jsonrpc
{
  template<typename T, typename D>
  void operator()(T& t, D)
  {
    t.get_aspect().template get<_parse_error_>()(t);
  }
  
};


struct aspect_error: fas::aspect< fas::type_list_n<
  fas::advice< _send_error_,       ad_send_error>, 
  fas::advice< _parse_error_,      ad_parse_error>, 
  fas::advice< _invalid_request_,  ad_invalid_request>, 
  fas::advice< _method_not_found_, ad_method_not_found>, 
  fas::advice< _invalid_params_,   ad_invalid_params>, 
  fas::advice< _internal_error_,   ad_internal_error>, 
  fas::advice< _server_error_,     ad_server_error>, 
  fas::alias<  _jsonrpc_error_,    _server_error_>, 
  fas::advice<  _not_jsonrpc_,      ad_not_jsonrpc>
>::type >
{};

}}}}
