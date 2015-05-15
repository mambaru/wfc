//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/iinterface.hpp>
#include <memory>
#include <string>

namespace wfc{

struct imodule;
struct global;

struct ipackage: iinterface
{
  virtual ~ipackage() {}
  virtual std::string version() = 0;
  virtual std::string description() = 0;
  virtual std::vector<std::string> modules() = 0;
  virtual void create( std::shared_ptr<global> ) = 0;
};

}
