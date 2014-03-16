#pragma once

#include <string>
#include <wfc/memory.hpp>

namespace wfc{ namespace jsonrpc{

class invalid_id
  : public std::exception
{
public:
  invalid_id(int)
  {
    
  }
};

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

struct invalid_params: error
{
  invalid_params()
    : error(-32602, "Invalid params")
  {};
  
};

inline std::unique_ptr<error> server_configuration_error()
{
  return std::make_unique<error>( -32606, "Server configuration error");
}


}}
