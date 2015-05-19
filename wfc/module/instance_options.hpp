#pragma once

#include <string>

namespace wfc{

template<typename DomainOptions>
struct instance_options: DomainOptions
{
  typedef DomainOptions basic_type;
  bool enabled = true;
  int  startup_priority = 0;
  int  shutdown_priority = 0;
  std::string name;
};

}
