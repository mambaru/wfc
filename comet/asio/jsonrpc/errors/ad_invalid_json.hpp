#pragma once

#include <comet/json/json.hpp>
#include <comet/logger.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct ad_invalid_json
{
  template<typename T, typename Itr>
  void operator()(T& t, const json::json_error& e, Itr beg, Itr end)
  {
    COMMON_LOG_ERROR(e.message(beg, end));
  }
};

}}}}
