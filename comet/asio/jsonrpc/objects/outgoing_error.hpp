#pragma once

#include <comet/asio/jsonrpc/objects/outgoing.hpp>
#include <comet/asio/types.hpp>
#include <comet/memory.hpp>
#include <memory>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

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



}}}}


