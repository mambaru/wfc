#pragma once

#include <wfc/jsonrpc/outgoing/outgoing.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_result
  : outgoing
{
  std::unique_ptr<T> result;
  ::wfc::io::data_ptr id;
};

  
}}

