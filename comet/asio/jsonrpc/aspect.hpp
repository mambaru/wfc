#pragma once

#include <comet/asio/jsonrpc/ad_invoke.hpp>
#include <comet/asio/jsonrpc/tags.hpp>
#include <fas/aop.hpp>


/*
-32700  Parse error       Invalid JSON was received by the server.An error occurred on the server while parsing the JSON text.
-32600  Invalid Request   The JSON sent is not a valid Request object.
-32601  Method not found  The method does not exist / is not available.
-32602  Invalid params    Invalid method parameter(s).
-32603  Internal error    Internal JSON-RPC error.
-32000 to -32099  Server error
                          Reserved for implementation-defined server-errors.
*/


/*
--> {"jsonrpc": "2.0", "method": "foobar", "id": 10}
<-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Procedure not found."}, "id": 10}

Procedure Call with invalid JSON:

--> {"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz"]
<-- {"jsonrpc": "2.0", "error": {"code": -32700, "message": "Parse error"}, "id": null}

Procedure Call with invalid JSON-RPC:

--> [1,2,3]
<-- {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid JSON-RPC."}, "id": null}

--> {"jsonrpc": "2.0", "method": 1, "params": "bar"}
<-- {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid JSON-RPC."}, "id": null}
*/

struct _error_;

struct error
{
  int code;
  std::string message;
};

struct ad_parse_error
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_error_>()(t, error{-32700,"Parse error"});
  }
};

struct ad_invalid_request
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_error_>()(t, error{-32600,"Invalid Request"});
  }
};

struct ad_method_not_found
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_error_>()(t, error{-32601,"Method not found"});
  }
};

struct ad_invalid_params
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_error_>()(t, error{-32602,"Invalid params"});
  }
};

struct ad_internal_error
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_error_>()(t, error{-32603,"Internal error"});
  }
};

struct ad_server_error
{
  template<typename T>
  void operator()(T& t, int code, std::string message)
  {
    t.get_aspect().template get<_error_>()(t, error{code,message});
  }
};

struct ad_error
{
  template<typename T>
  void operator()(T& t, const error& err)
  {
    t.get_aspect().template get<_error_>()(t, error{ err.code, err.message} );
  }
};

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct aspect: fas::aspect< fas::type_list_n<
  fas::advice<_invoke_, ad_invoke>,
  fas::stub<_invalid_json_>,
  fas::alias<_input_, _invoke_>,
  fas::group< basic::_initialize_, _invoke_>
  //fas::stub<_not_jsonrpc_>
>::type >
{};

}}}}
