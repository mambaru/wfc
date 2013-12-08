#pragma once
#include <comet/asio/types.hpp>
#include <comet/memory.hpp>
#include <memory>
namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct outgoing
{
  typedef char version_type[4];
  version_type version;
  outgoing()
    : version{'2','.','0','\0'}
  {}
};
  
}}}}
