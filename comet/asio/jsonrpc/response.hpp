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
    , id(0)
  {
    
  }
};

template<typename T>
struct notify
{
  typedef char version_type[4];
  version_type version;
  std::unique_ptr<T> result;
  notify()
    : version{'2','.','0','\0'}
  {
    
  }
};

template<typename T>
struct request
{
  typedef char version_type[4];
  version_type version;
  int id;
  std::string method;
  std::unique_ptr<T> params;
  request()
    : version{'2','.','0','\0'}
    , id(0)
  {
    method.reserve(20);
  }
};


}}}}


