//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/iinterface.hpp>
#include <string>

namespace wfc{

struct iconfig: iinterface
{
  virtual ~iconfig() {}
  virtual void reload_and_reconfigure() = 0;
  virtual void load_and_parse(std::string path) = 0;
  virtual std::string get_config(std::string name) = 0;
  virtual std::string generate_and_write(std::string type, std::string path) = 0;
};

}
