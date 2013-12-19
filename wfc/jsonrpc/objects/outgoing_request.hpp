#pragma once

#include <wfc/jsonrpc/objects/outgoing.hpp>
#include <memory>
#include <string>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_request
  : outgoing
{
  std::string method;
  std::unique_ptr<T> params;
  int id;
  outgoing_request()
    : outgoing()
    , id(-1)
  {
  }
};

}}

