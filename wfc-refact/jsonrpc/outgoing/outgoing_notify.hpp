#pragma once

#include <wfc/jsonrpc/outgoing/outgoing.hpp>
#include <memory>
#include <string>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_notify
  : outgoing
{
  // TODO: оптимизоровать method, на const char (см. json)
  std::string method;
  std::unique_ptr<T> params;
};

}}

