//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wfc/wfc_exit.hpp>
#include <wfc/core/instance_args.hpp>
#include <sstream>
#include <memory>

namespace wfc{

instance_args::instance_args( std::string name )
  : _name(name)
{}
    
instance_args::instance_args( std::string name, args_map args )
  : _name( std::move(name) )
  , _args( std::move(args) )
{
}
  
void instance_args::insert( args_map args )
{
  for (const auto& i : args )
  {
    _args[i.first] = i.second;
  }
}

const std::string& instance_args::name() const 
{
  return _name;
}
  
std::string instance_args::get(const std::string& key, bool required) const
{
  auto itr = _args.find(key);
  if ( itr != _args.end() )
    return itr->second;
  if ( required )
  {
    std::stringstream ss;
    ss << "instance '" << _name << "' required arg '" << key << "'";
    ::wfc_exit_with_error( ss.str() );
  }
  return std::string();
}
  
bool instance_args::has(const std::string& key) const
{
  return _args.count(key) != 0;
}
  
instance_args::args_map instance_args::args() const
{
  return _args;
}
  
}
