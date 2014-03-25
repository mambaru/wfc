//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <memory>

namespace wfc{ namespace pubsub{

typedef std::string identity_t;
typedef std::vector<char> data_t;
typedef std::unique_ptr<data_t> data_ptr;
typedef size_t cursor_t;
typedef std::string key_t;

}}


