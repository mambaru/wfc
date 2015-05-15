#pragma once

#include <wfc/jsonrpc/outgoing/outgoing.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_error: outgoing
{
  typedef ::wfc::io::data_type data_type;
  std::unique_ptr<T> error;
  std::unique_ptr<data_type> id;
  outgoing_error()
    : outgoing()
    , id(nullptr)
  {}
};
}}

