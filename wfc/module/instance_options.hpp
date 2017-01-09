#pragma once

#include <string>
//#include <wfc/workflow.hpp>

namespace wfc{

#warning TODO:  
struct nostat{};

template<typename DomainStatOpt = nostat >
struct base_statistics_options;

template<>
struct base_statistics_options<nostat>: nostat
{
  static constexpr bool enabled = false; 
  bool disabled = true;
  std::string target;
};

template<typename DomainStatOpt>
struct base_statistics_options: DomainStatOpt
{
  static constexpr bool enabled = true;
  bool disabled = true;
  std::string target;
};

template<typename DomainStatOpt=nostat>
struct base_instance_options
{
  bool enabled = true;
  bool suspend = false;
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
  std::string workflow;
  base_statistics_options<DomainStatOpt> statistics;
};

template<typename DomainOptions, typename DomainStatOpt=nostat>
struct domain_instance_options
  : base_instance_options<DomainStatOpt>
  , DomainOptions
{
  typedef base_instance_options<DomainStatOpt> instance_options;
  typedef DomainOptions domain_options;
};


}
