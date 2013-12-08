#pragma once

#include <comet/json/name.hpp>
#include <comet/json/json.hpp>
#include <comet/asio/jsonrpc/objects/outgoing.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{


struct version_member
{
  NAME(jsonrpc);
  typedef json::member<n_jsonrpc, outgoing, outgoing::version_type, &outgoing::version> type;
};  

}}}}


