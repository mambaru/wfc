//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <string>
#include <map>

namespace wfc{

struct iconfig: iinterface
{
  virtual ~iconfig() {}
  virtual bool reload_and_reconfigure() = 0;
  virtual bool load_and_parse(std::string path) = 0;
  virtual std::string get_config(std::string name) = 0;
  typedef std::map<std::string, std::string> generate_options;
  virtual bool generate_config( const generate_options& go, const std::string& path, std::string& result) = 0;
};

}
