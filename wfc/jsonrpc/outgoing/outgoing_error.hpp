#pragma once

#include <wfc/jsonrpc/outgoing/outgoing.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct outgoing_error: outgoing
{
  std::unique_ptr<T> error;
  std::unique_ptr< ::wfc::io::data_type> id;
  outgoing_error()
    : outgoing()
    , id(nullptr)
  {}
};
}}

