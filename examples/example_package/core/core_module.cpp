#include <wfc/name.hpp>
#include <wfc/module/component_list.hpp>
#include "core_module.hpp"
#include "core/core_singleton.hpp"
#include "startup/startup_singleton.hpp"

namespace
{
  WFC_NAME2(module_name, "core-module")
  struct impl: 
    wfc::component_list<
      module_name,
      core_singleton,
      startup_singleton
    >
  {};
}

core_module::core_module()
  : wfc::module(std::make_shared<impl>())
{
  
}

