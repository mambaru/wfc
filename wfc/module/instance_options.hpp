#pragma once
#include <string>
#include <set>

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
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
  std::string workflow;
  std::set<int> cpu;
  stat_options statistics;
};

/**
 * @brief domain_config
 */
template<typename CustomOptions, typename StatOpt /*=nostat */>
struct domain_config_t
  : domain_options_t<StatOpt>
  , CustomOptions
{
  typedef domain_options_t<StatOpt> domain_options;
  typedef CustomOptions custom_options;
  typedef typename domain_options::stat_options stat_options;
};


}
