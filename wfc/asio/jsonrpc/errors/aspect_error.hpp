#pragma once

#include <wfc/asio/jsonrpc/errors/ad_invalid_json.hpp>
#include <wfc/asio/jsonrpc/errors/ad_send_error.hpp>
#include <wfc/asio/jsonrpc/errors/ad_jsonrpc_error.hpp>
#include <wfc/asio/jsonrpc/errors/ad_not_jsonrpc.hpp>
#include <wfc/asio/jsonrpc/errors/error.hpp>
#include <wfc/asio/jsonrpc/errors/tags.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{ namespace jsonrpc{

struct aspect_error: fas::aspect< fas::type_list_n<
  fas::advice< _send_error_,       ad_send_error>, 
  fas::advice< _parse_error_,      ad_parse_error>, 
  fas::advice< _invalid_request_,  ad_invalid_request>, 
  fas::advice< _method_not_found_, ad_method_not_found>, 
  fas::advice< _invalid_params_,   ad_invalid_params>, 
  fas::advice< _internal_error_,   ad_internal_error>,
  fas::advice< _invalid_id_,       ad_invalid_id>, 
  fas::advice< _method_not_impl_,  ad_method_not_impl>,
  fas::advice< _server_error_,     ad_server_error>, 
  fas::alias<  _jsonrpc_error_,    _server_error_>, 
  fas::advice< _not_jsonrpc_,      ad_not_jsonrpc>,
  fas::advice< _invalid_json_,     ad_invalid_json>
>::type >
{};

}}}
