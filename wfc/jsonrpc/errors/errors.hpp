#pragma once

#include <string>

namespace wfc{ namespace jsonrpc{
  
struct error
{
  int code;
  std::string message;
  
  error()
    : code()
    , message()
  {}
  
  error(int code, std::string message)
    : code(code)
    , message(message)
  {}
  
};

// -32602, "Invalid params"


struct parse_error: error
{
  parse_error()
    : error(-32700, "Parse error.")
  {}
};

struct invalid_request: error
{
  invalid_request()
    : error(-32600, "Invalid Request.")
  {}
};

struct procedure_not_found: error
{
  procedure_not_found()
    : error(-32601, "Procedure not found.")
  {}
};

struct invalid_params: error
{
  invalid_params()
    : error(-32602, "Invalid params.")
  {}
};

struct internal_error: error
{
  internal_error()
    : error(-32603, "Internal error.")
  {}
};

struct method_not_impl: error
{
  method_not_impl()
    : error(-32001, "Method not implementation.")
  {}
};

struct server_error: error
{
  server_error()
    : error(-32000, "Server error.")
  {}

  server_error( const std::string& what)
    : error(-32002, std::string("Server error: ") + what + ".")
  {}
};

}}
