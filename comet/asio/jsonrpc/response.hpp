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
  
template<typename T>
struct outgoing_response: outgoing
{
  int id;
  std::unique_ptr<T> result;
  outgoing_response()
    : outgoing()
    , id(0)
  {
    
  }
};

template<typename T>
struct outgoing_notify: outgoing
{
  std::unique_ptr<T> params;
  outgoing_notify()
    : outgoing()
  {
    
  }
};

template<typename T>
struct outgoing_request: outgoing
{
  std::string method;
  std::unique_ptr<T> params;
  int id;
  outgoing_request()
    : outgoing()
    , id(0)
  {
    method.reserve(20);
  }
};

// {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null}

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


