#pragma once

#include <wfc/jsonrpc/errors/tags.hpp>

namespace wfc{ namespace jsonrpc{

struct ad_not_jsonrpc
{
  template<typename T, typename D>
  void operator()(T& t, D)
  {
    t.get_aspect().template get<_parse_error_>()(t);
  }
};

}}
