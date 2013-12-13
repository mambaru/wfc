#pragma once

#include <wfc/json/name.hpp>
#include <wfc/json/json.hpp>
#include <wfc/inet/jsonrpc/objects/outgoing.hpp>

namespace wfc{ namespace inet{ namespace jsonrpc{


struct version_member
{
  NAME(jsonrpc)
  typedef json::member<n_jsonrpc, outgoing, outgoing::version_type, &outgoing::version> type;
};  

}}}


