#pragma once

#include <comet/asio/jsonrpc/errors/error.hpp>
#include <comet/json/name.hpp>
#include <comet/json/json.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

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

}}}}
