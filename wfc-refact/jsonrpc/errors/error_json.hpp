#pragma once

#include <wfc/json/json.hpp>
#include <fas/type_list.hpp>


namespace wfc{ namespace jsonrpc{

struct error_json
{
  JSON_NAME(code)
  JSON_NAME(message)
  
  typedef json::object<
    error, 
    fas::type_list_n<
      json::member<n_code,    error, int,         &error::code>,
      json::member<n_message, error, std::string, &error::message>
    >::type
  > type;

  typedef error target; 
  typedef type::member_list member_list;
  typedef type::serializer  serializer;
};
  
}}
