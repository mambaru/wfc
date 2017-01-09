#pragma once

#include <wfc/module/basic_component.hpp>

namespace wfc{

template< typename Name, typename Instance, typename DomainJson, int Features = component_features::Multiton >
class multiton
  : public basic_component<
      Name, 
      Instance, 
      DomainJson, 
      int(component_features::Multiton) | Features
    > 
{};

}
