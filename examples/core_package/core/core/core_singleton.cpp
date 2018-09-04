#include <wfc/name.hpp>
#include <wfc/module/singleton.hpp>
#include <wfc/module/instance.hpp>
#include "core_singleton.hpp"
#include "core_domain.hpp"
#include "core_options_json.hpp"

namespace
{
  WFC_NAME2(core_name, "core")
  struct core_impl: wfc::singleton<core_name, wfc::instance<core_domain>, core_options_json>{};
}

core_singleton::core_singleton()
  : wfc::component(std::make_shared<core_impl>())
{}

