#pragma once

#include <comet/asio/types.hpp>
#include <comet/memory.hpp>
#include <memory>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct incoming
{
  typedef data_type::iterator iterator;
  typedef std::pair<iterator, iterator> pair_type;
  pair_type method;
  pair_type params;
  pair_type result;
  pair_type error;
  std::unique_ptr<int> id;
  
  incoming()
    : id(std::make_unique<int>(-1))
  {
    // method.reserve(64) ;
  }
};

}}}}


