#pragma once

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>
#include "multi_instance_config.hpp"
namespace wfc{

struct multi_instance_config_json
{
  typedef json::array<
    std::unordered_map<
      json::value<std::string>,
      json::raw_value<std::string>
    >
  >::serializer serializer;
};

}
