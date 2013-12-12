#pragma once

#include <wfc/inet/jsonrpc/objects/outgoing.hpp>
#include <wfc/inet/types.hpp>
#include <wfc/memory.hpp>
#include <memory>
namespace wfc{ namespace inet{ namespace jsonrpc{

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

}}}
