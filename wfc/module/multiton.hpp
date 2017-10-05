#pragma once

#include <wfc/module/basic_component.hpp>

namespace wfc{

template<
  typename Name,
  typename Instance,
  typename DomainJson,
  int Features = component_features::Multiton,
  typename StatJson = typename std::conditional< 
                        std::is_same<typename Instance::statistics_options, nostat>::value, 
                        nostat_json, defstat_json 
                      >::type
>
class multiton
  : public basic_component<
      Name, 
      Instance, 
      DomainJson, 
      int(component_features::Multiton) | Features,
      StatJson
  > 
{};

}
