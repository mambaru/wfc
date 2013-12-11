#pragma once

#include <comet/asio/jsonrpc/errors/tags.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct ad_not_jsonrpc
{
  template<typename T, typename D>
  void operator()(T& t, D)
  {
    t.get_aspect().template get<_parse_error_>()(t);
  }
};

}}}}
