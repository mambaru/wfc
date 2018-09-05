#include <wfc/module/module_list.hpp>
#include "example_package.hpp"
#include "core/core_module.hpp"
#include "example/example_module.hpp"
#include "example_build_info.h"

namespace 
{
  struct impl: 
    wfc::module_list<
      example_build_info,
      core_module,
      example_module
    >
  {
  };
}

example_package::example_package()
  : wfc::package(std::make_shared<impl>())
{
  
}

