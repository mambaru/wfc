#pragma once

#include <string>

namespace wfc{

template<typename DomainOptions>
struct instance_options: DomainOptions
{
  typedef DomainOptions domain_options;
  bool enabled = true;
  bool suspend = false;
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
};

}
