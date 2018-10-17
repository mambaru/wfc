#include <wfc/name.hpp>
#include <wfc/module/singleton.hpp>
#include <wfc/module/instance.hpp>
#include "startup_singleton.hpp"
#include "startup_domain.hpp"
#include "startup_options_json.hpp"

namespace
{
  WFC_NAME2(startup_name, "startup")
  struct startup_impl: wfc::singleton<startup_name, wfc::instance<startup_domain>, startup_options_json>{};
}

startup_singleton::startup_singleton()
  : wfc::component(std::make_shared<startup_impl>())
{}

