#pragma once

#include <comet/asio/jsonrpc/objects/outgoing.hpp>
#include <comet/asio/types.hpp>
#include <comet/memory.hpp>
#include <memory>
namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

template<typename T>
struct outgoing_notify: outgoing
{
  std::string method;
  std::unique_ptr<T> params;
  outgoing_notify()
    : outgoing()
  {
    
  }
};

}}}}
