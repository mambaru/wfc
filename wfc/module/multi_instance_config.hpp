//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <unordered_map>

namespace wfc{

struct multi_instance_config: std::unordered_map<std::string, std::string> {};

}
