#pragma once

#include <comet/asio/jsonrpc/objects/outgoing.hpp>
#include <memory>
#include <string>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

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

}}}}

