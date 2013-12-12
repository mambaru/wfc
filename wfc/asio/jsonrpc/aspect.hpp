#pragma once

#include <wfc/asio/jsonrpc/ad_invoke.hpp>
#include <wfc/asio/jsonrpc/ad_send_json.hpp>
#include <wfc/asio/jsonrpc/errors/aspect_error.hpp>
#include <wfc/asio/jsonrpc/tags.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace inet{ namespace jsonrpc{

struct aspect: fas::aspect< fas::type_list_n<
  aspect_error, 
  fas::advice<_send_json_, ad_send_json>,
  fas::advice<_invoke_, ad_invoke>,
  fas::alias<_input_, _invoke_>,
  fas::group< basic::_initialize_, _invoke_>, 
  fas::stub<_method_stat_>
>::type >
{};

}}}
