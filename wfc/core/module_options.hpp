//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/logger.hpp>

#include <map>
#include <string>
#include <cstdlib>

namespace wfc{
  
class module_options
{
public:

  typedef std::map<std::string, std::string> options_map;
  typedef std::map<std::string, options_map> module_options_map;
  
  module_options(){}
  
  module_options(const module_options_map& po)
    : _module_options(po)
  {}
  
  std::string get(const std::string& name, const std::string& key, bool required = false) const
  {
    auto itr1 = _module_options.find(name);
    if ( itr1 != _module_options.end() )
    {
      auto itr2 = itr1->second.find(key);
      if ( itr2 != itr1->second.end() )
      {
        return itr2->second;
      }
    }
    
    if ( required )
    {
      CONFIG_LOG_FATAL("Required option '" << key << "' for module '" << name << "'")
      std::abort();
    }
    return std::string();
  }
private:
  module_options_map _module_options;
};

}
