#pragma once
#include <comet/asio/types.hpp>
#include <comet/memory.hpp>
#include <memory>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

template<typename T>
struct outgoing_request: outgoing
{
  std::string method;
  std::unique_ptr<T> params;
  int id;
  outgoing_request()
    : outgoing()
    , id(0)
  {
    method.reserve(20);
  }
};

}}}}


