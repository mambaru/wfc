//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wfc/wfc_exit.hpp>
#include <wfc/core/extended_args.hpp>
#include <sstream>

namespace wfc{

extended_args::extended_args(){}

extended_args::extended_args(const extended_args_map& po)
  : _extended_args(po)
{}

void extended_args::insert( std::map<std::string, std::map<std::string, std::string> > args)
{
  for (auto& ins : args)
  {
    this->insert(ins.first, instance_args(ins.first, std::move(ins.second)) );
  }
}

void extended_args::insert(const std::string& name, instance_args args)
{
  std::lock_guard<std::mutex> lk(_mutex);

  auto itr = _extended_args.find(name);
  if ( itr == _extended_args.end() )
  {
    _extended_args.insert( std::make_pair(name, args) );
  }
  else
  {
    itr->second.insert( args.args() );
  }
}

instance_args extended_args::get(const std::string& name, bool required ) const
{
  std::lock_guard<std::mutex> lk(_mutex);

  auto itr = _extended_args.find(name);

  if ( itr != _extended_args.end() )
    return itr->second;

  if ( required )
  {
    std::stringstream ss;
    ss << "instance '" << name << "' required args ";
    ::wfc_abort( ss.str() );
  }
  return instance_args(name);
}

bool extended_args::has(const std::string& name) const
{
  std::lock_guard<std::mutex> lk(_mutex);
  return _extended_args.count(name) != 0;
}

void extended_args::clear()
{
  std::lock_guard<std::mutex> lk(_mutex);
  return _extended_args.clear();
}

}
