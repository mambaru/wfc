//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wfc/module/statistics_options.hpp>
#include <string>
#include <set>

namespace wfc{

/**
 * @brief basic_instance_options
 */
template<typename StatOpt /*= nostat*/>
struct domain_options_t
{
  typedef statistics_options_t<StatOpt> stat_options;
  static constexpr bool statistics_enabled = stat_options::enabled;
  bool enabled = true;
  bool suspend = false;
  bool tracking = false;
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
  std::string workflow;
  std::set<int> cpu;
  stat_options statistics;
};

}
