#pragma once

#include <wfc/json/json.hpp>
#include <wfc/logger.hpp>

namespace wfc{ namespace inet{ namespace jsonrpc{

struct ad_invalid_json
{
  template<typename T, typename Itr>
  void operator()(T& t, const json::json_error& e, Itr beg, Itr end)
  {
    // TODO: вынести в wfc::jsonrpc
    COMMON_LOG_ERROR(e.message(beg, end));
  }
};

}}}
