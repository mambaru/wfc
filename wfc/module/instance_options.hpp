#pragma once
#include <string>
#include <set>

namespace wfc{

struct nostat{};
struct defstat{};

template<typename StatOpt /*= nostat*/ >
struct statistics_options;

template<>
struct statistics_options<nostat>: nostat
{
  static constexpr bool enabled = false; 
  bool disabled = true;
  std::string target;
};

template<typename StatOpt>
struct statistics_options: StatOpt
{
  static constexpr bool enabled = true;
  bool disabled = true;
  std::string target;
};

template<typename StatOpt /*= nostat*/>
struct basic_instance_options
{
  typedef ::wfc::statistics_options<StatOpt> statistics_options;
  static constexpr bool statistics_enabled = statistics_options::enabled;
  bool enabled = true;
  bool suspend = false;
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
  std::string workflow;
  std::set<int> cpu;
  statistics_options statistics;
};

template<typename DomainOptions, typename StatOpt /*=nostat */>
struct instance_options
  : basic_instance_options<StatOpt>
  , DomainOptions
{
  typedef basic_instance_options<StatOpt> basic_options;
  typedef DomainOptions domain_options;
  typedef typename basic_options::statistics_options statistics_options;
};


}
