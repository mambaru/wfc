#pragma once

#include <wfc/stat/istat.hpp>
#include <wrtstat/wrtstat_options.hpp>
#include <memory>

namespace wfc{

struct stat_options: public ::wrtstat::wrtstat_options
{
  std::vector<std::string> prefixes;
};

}
