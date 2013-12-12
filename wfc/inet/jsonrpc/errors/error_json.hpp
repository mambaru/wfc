#pragma once

#include <wfc/inet/jsonrpc/errors/error.hpp>
#include <wfc/json/name.hpp>
#include <wfc/json/json.hpp>

namespace wfc{ namespace inet{ namespace jsonrpc{

struct error_json
{
  NAME(code);
  NAME(message);
  
  typedef json::object<
    error, 
    fas::type_list_n<
      json::member<n_code,    error, int,         &error::code>,
      json::member<n_message, error, std::string, &error::message>
    >::type
  > type;

  typedef type::serializer serializer;
};

}}}
