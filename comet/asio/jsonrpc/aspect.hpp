#pragma once

#include <comet/asio/jsonrpc/ad_invoke.hpp>
#include <comet/asio/jsonrpc/ad_send_json.hpp>
#include <comet/asio/jsonrpc/errors/aspect_error.hpp>
#include <comet/asio/jsonrpc/tags.hpp>
#include <fas/aop.hpp>


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct aspect: fas::aspect< fas::type_list_n<
  aspect_error, 
  fas::advice<_send_json_, ad_send_json>,
  fas::advice<_invoke_, ad_invoke>,
  //fas::stub<_invalid_json_>,
  fas::alias<_input_, _invoke_>,
  fas::group< basic::_initialize_, _invoke_>
  //fas::stub<_not_jsonrpc_>
  
>::type >
{};

}}}}
