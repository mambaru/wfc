#pragma once

#include <wfc/io/types.hpp>
#include <utility>

namespace wfc{ namespace jsonrpc{
  
struct incoming
{
  typedef ::wfc::io::data_type data_type;
  typedef data_type::iterator iterator;
  typedef std::pair<iterator, iterator> pair_type;
  pair_type method;
  pair_type params;
  pair_type result;
  pair_type error;
  pair_type id;
};

}}
