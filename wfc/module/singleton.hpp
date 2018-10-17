#pragma once

#include <wfc/module/basic_component.hpp>

namespace wfc{

/**
 * @brief singleton
 */
template<
  typename Name,
  typename Instance,
  typename DomainJson,
  int Features = component_features::Defaults,
  typename StatJson = nostat_json
>
class singleton
  : public basic_component<
      Name,
      Instance,
      DomainJson,
      int(component_features::Singleton) | Features, 
      StatJson
    > {};

}

