#pragma once

#include <comet/asio/jsonrpc/objects/outgoing.hpp>
#include <memory>
#include <string>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

template<typename T>
struct outgoing_response
  : outgoing
{
  int id;
  std::unique_ptr<T> result;
  outgoing_response()
    : outgoing()
    , id(0)
  {
    
  }
};

}}}}

