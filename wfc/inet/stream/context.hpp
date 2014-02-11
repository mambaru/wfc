#pragma once

#include <wfc/inet/conn/connection_context.hpp>

namespace wfc{ namespace inet{ namespace stream{


template<typename Base = empty_context>
struct context: connection_context<Base>
{
  size_t outgoing_warning;
  size_t outgoing_limit;
  bool shutdown;
  context()
    : connection_context<Base>()
    , outgoing_warning(0)
    , outgoing_limit(0)
    , shutdown(false)
  {}
};

}}}
