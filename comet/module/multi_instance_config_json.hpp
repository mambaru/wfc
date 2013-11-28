#pragma once

#include <comet/json/json.hpp>
#include <comet/json/name.hpp>
#include "multi_instance_config.hpp"
namespace mamba{ namespace comet{

struct multi_instance_config_json
{
  typedef json::array<
    std::unordered_map<
      json::value<std::string>,
      json::raw_value<std::string>
    >
  >::serializer serializer;
};

}}
