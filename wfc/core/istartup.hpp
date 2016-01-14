//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <memory>

namespace wfc{

struct global;
  
struct istartup: iinterface
{
  virtual ~istartup() {}
  virtual bool startup(int argc, char* argv[], std::string helpstring) = 0;
};

}
