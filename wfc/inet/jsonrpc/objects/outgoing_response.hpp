#pragma once

#include <wfc/inet/jsonrpc/objects/outgoing.hpp>
#include <memory>
#include <string>

namespace wfc{ namespace inet{ namespace jsonrpc{

template<typename T>
struct outgoing_response
  : outgoing
{
  std::unique_ptr<T> result;
  int id;
  outgoing_response()
    : outgoing()
    , id(-1)
  {
    
  }
};

}}}

