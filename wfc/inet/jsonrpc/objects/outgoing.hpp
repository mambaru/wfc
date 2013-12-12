#pragma once
#include <wfc/inet/types.hpp>
#include <wfc/memory.hpp>
#include <memory>
namespace wfc{ namespace inet{ namespace jsonrpc{

struct outgoing
{
  typedef char version_type[4];
  version_type version;
  outgoing()
    : version{'2','.','0','\0'}
  {}
};
  
}}}
