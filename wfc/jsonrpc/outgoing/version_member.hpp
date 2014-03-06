#pragma once

#include <wfc/jsonrpc/outgoing/outgoing.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace jsonrpc{

struct version_member
{
  JSON_NAME(jsonrpc)
  typedef json::member<n_jsonrpc, outgoing, outgoing::version_type, &outgoing::version> type;
};  

}}

