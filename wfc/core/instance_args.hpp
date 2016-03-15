//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <map>
#include <string>

namespace wfc{

class instance_args
{
public:
  typedef std::map<std::string, std::string> args_map;
  
  instance_args( std::string name );
    
  instance_args( std::string name, args_map args );
  
  void insert( args_map args );

  const std::string& name() const;
  
  std::string get(const std::string& key, bool required = false) const;
  
  bool has(const std::string& key) const;
  
  args_map args() const;
  
private:
  std::string _name;
  args_map _args;
};
  
}
