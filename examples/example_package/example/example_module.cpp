#include <wfc/name.hpp>
#include <wfc/module/component_list.hpp>
#include "example_module.hpp"
#include "domain/example_multiton.hpp"


namespace
{
  WFC_NAME2(module_name, "example-module")
  struct impl: 
    wfc::component_list<
      module_name,
      example_multiton
    >
  {};
}

example_module::example_module()
  : wfc::module(std::make_shared<impl>())
{
  
}
