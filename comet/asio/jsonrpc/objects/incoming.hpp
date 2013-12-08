#pragma once

#include <comet/asio/types.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct incoming
{
  typedef data_type::iterator iterator;
  typedef std::pair<iterator, iterator> pair_type;
  // TODO: id -> pair_type
  int id;
  pair_type method;
  pair_type params;
  pair_type result;
  pair_type error;
  
  incoming()
    : id(-1)
  {
    // method.reserve(64) ;
  }
};

}}}}


