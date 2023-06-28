#pragma once

#include <wrtstat/wrtstat_options.hpp>

namespace wfc{ namespace statistics{

struct statistics_options
  : wrtstat::wrtstat_options
{
  std::vector<std::string> prefixes;
};

}}

