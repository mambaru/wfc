//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "multi_instance_config.hpp"

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

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
