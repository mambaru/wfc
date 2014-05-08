#pragma once

#include <wfc/jsonrpc/objects/outgoing.hpp>

#include <wfc/json/name.hpp>
#include <wfc/json/json.hpp>


namespace wfc{ namespace jsonrpc{


struct version_member
{
  FAS_NAME(jsonrpc)
  typedef json::member<n_jsonrpc, outgoing, outgoing::version_type, &outgoing::version> type;
};  

}}
