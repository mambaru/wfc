#pragma once

#include <string>
#include <wfc/workflow.hpp>

namespace wfc{

struct base_instance_options
{
  bool enabled = true;
  bool suspend = false;
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
  workflow_options workflow;
};

template<typename DomainOptions>
struct domain_instance_options
  : base_instance_options
  , DomainOptions
{
  typedef DomainOptions domain_options;
};


}
