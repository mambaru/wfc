#include <wfc/module/module_list.hpp>
#include "core_package.hpp"
#include "core/core_module.hpp"
#include "core_build_info.h"

namespace 
{
  struct impl: 
    wfc::module_list<
      core_build_info,
      core_module
    >
  {
  };
}

core_package::core_package()
  : wfc::package(std::make_shared<impl>())
{
  
}

