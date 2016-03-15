//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/instance_args.hpp>
#include <map>
#include <string>
#include <mutex>

namespace wfc{
  
class extended_args
{
public:

  typedef std::map<std::string, instance_args> extended_args_map;
  
  extended_args();
  
  extended_args(const extended_args_map& po);
  
  void insert(const std::string& name, instance_args args);
  
  instance_args get(const std::string& name, bool required = false) const;

  bool has(const std::string& name) const;
  
private:
  extended_args_map _extended_args;
  mutable std::mutex _mutex;
};

}
