#include <wfc/name.hpp>
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include "example_multiton.hpp"
#include "example_domain.hpp"
#include "example_options_json.hpp"

namespace
{
  WFC_NAME2(example_name, "example")
  struct example_impl: wfc::multiton<example_name, wfc::instance<example_domain>, example_options_json>{};
}

example_multiton::example_multiton()
  : wfc::component(std::make_shared<example_impl>())
{}

