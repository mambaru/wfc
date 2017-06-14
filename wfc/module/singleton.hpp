#pragma once

#include <wfc/module/basic_component.hpp>

namespace wfc{

template<
  typename Name,
  typename Instance,
  typename DomainJson,
  int Features = 0,
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

