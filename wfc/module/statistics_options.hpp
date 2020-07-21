//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <string>

namespace wfc{

struct nostat{};
struct defstat{};

template<typename StatOpt /*= nostat*/ >
struct statistics_options_t;

template<>
struct statistics_options_t<nostat>: nostat
{
  static constexpr bool enabled = false;
  bool disabled = true;
  std::string target;
};

template<typename StatOpt>
struct statistics_options_t: StatOpt
{
#ifdef WFC_ENABLE_STAT
  static constexpr bool enabled = true;
#else
  static constexpr bool enabled = false;
#endif
  bool disabled = true;
  std::string target;
};

}
