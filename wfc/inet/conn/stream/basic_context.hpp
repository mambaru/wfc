#pragma once

#include <wfc/inet/conn/basic_context.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace stream{


template<typename Base = empty_context>
struct basic_context: wfc::inet::conn::basic_context<Base>
{
  size_t outgoing_warning;
  size_t outgoing_limit;
  bool shutdown;
  basic_context()
    : wfc::inet::conn::basic_context<Base>()
    , outgoing_warning(0)
    , outgoing_limit(0)
    , shutdown(false)
  {}
};

}}}}
