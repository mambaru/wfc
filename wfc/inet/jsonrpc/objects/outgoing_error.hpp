#pragma once

#include <wfc/inet/jsonrpc/objects/outgoing.hpp>
#include <wfc/inet/types.hpp>
#include <wfc/memory.hpp>
#include <memory>

namespace wfc{ namespace inet{ namespace jsonrpc{

template<typename T>
struct outgoing_error: outgoing
{
  std::unique_ptr<T> error;
  std::unique_ptr<int> id;
  outgoing_error()
    : outgoing()
    , id(std::make_unique<int>(-1))
  {}

};



}}}


