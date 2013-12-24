#pragma once

#include <string>

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

}}
