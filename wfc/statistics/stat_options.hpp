#pragma once

#include <wrtstat/wrtstat_options.hpp>
#include <vector>
#include <string>

namespace wfc{

struct stat_options: public ::wrtstat::wrtstat_options
{
  /*
  std::string time_suffix;
  std::string size_suffix;
  std::vector<std::string> prefixes;
  */
};

}
