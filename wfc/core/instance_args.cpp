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

instance_args::instance_args( const std::string& nm)
  : _name(nm)
{}

instance_args::instance_args( const std::string& nm, const args_map& rgm )
  : _name( std::move(nm) )
  , _args( std::move(rgm) )
{
}

void instance_args::insert( const args_map& rgm )
{
  for (const auto& i : rgm )
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
    ::wfc_abort( ss.str() );
  }
  return std::string();
}

bool instance_args::has(const std::string& key) const
{
  return _args.count(key) != 0;
}

const instance_args::args_map& instance_args::args() const
{
  return _args;
}

}
