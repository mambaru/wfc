#pragma once

#include <wfc/asio/jsonrpc/errors/tags.hpp>

namespace wfc{ namespace inet{ namespace jsonrpc{

struct ad_not_jsonrpc
{
  template<typename T, typename D>
  void operator()(T& t, D)
  {
    t.get_aspect().template get<_parse_error_>()(t);
  }
};

}}}
