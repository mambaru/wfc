#pragma once

#include <wfc/module/basic_component.hpp>

namespace wfc{

/**
 * @brief multiton
 */
template<
  typename Name,
  typename Instance,
  typename DomainJson,
  int Features = component_features::Defaults,
  typename StatJson = typename std::conditional< 
                        std::is_same<typename Instance::customstat_options, nostat>::value, 
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
