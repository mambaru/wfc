#pragma once
#include <comet/asio/types.hpp>
#include <memory>
namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

template<typename T>
struct response
{
  typedef char version_type[4];
  version_type version;
  int id;
  std::unique_ptr<T> result;
  response()
    : version{'2','.','0','\0'}
  {
    
  }
};

}}}}


