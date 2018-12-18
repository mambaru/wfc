//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//


#include "object_registry.hpp"

namespace wfc{

void object_registry::set_object(const std::string& prefix, const std::string& name, std::shared_ptr<iinterface> item, bool nomark )
{
  if (name.empty() )
  {
    DOMAIN_LOG_FATAL("wfc::registry::set: empty name " )
    return;
  }
  std::lock_guard<mutex_type> lk(_mutex);
  key_type key(prefix, name);
  auto itr = _registry_map.find( key );
  if ( itr == _registry_map.end() )
  {
    _registry_map.insert( std::make_pair(key, item));
    if ( !nomark ) 
        ++_dirty;
  }
  else
  {
    if ( itr->second!=item && !nomark)
    {
        ++_dirty;
    }
    itr->second = item;
  }
}
  
void object_registry::set_target(const std::string& name, std::shared_ptr<iinterface> item, bool nomark )
{
  this->set_object("", name, item, nomark);
}

void object_registry::erase(const std::string& prefix, const std::string& name)
{
  interface_ptr ptr;
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _registry_map.find(key_type(prefix,name));
    if ( itr != _registry_map.end() )
    {
      ptr = itr->second;
      _registry_map.erase( itr );
      ++_dirty;
    }
  }
}

void object_registry::erase(const std::string& name)
{
  this->erase("", name);
}
  
void object_registry::clear()
{
  registry_map rm;
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _registry_map.swap(rm);
    ++_dirty;
  }
  rm.clear();
}
  
size_t object_registry::size() const
{
  std::lock_guard<mutex_type> lk(_mutex);
  return _registry_map.size();
}

int object_registry::dirty() const
{
  std::lock_guard<mutex_type> lk(_mutex);
  return _dirty;
}
  
int object_registry::reset_dirty()
{
  std::lock_guard<mutex_type> lk(_mutex);
  int result = _dirty;
  _dirty = 0;
  return result;
}

std::shared_ptr<iinterface> object_registry::get_(const std::string& prefix, const std::string& name, bool disabort) const
{
  if ( name.empty() )
    return nullptr;

  bool not_found = false;
  std::shared_ptr<iinterface> result = nullptr;
  registry_map::const_iterator itr;
  {
    std::lock_guard<mutex_type> lk(_mutex);
    itr = _registry_map.find( key_type(prefix, name) );
    if ( itr == _registry_map.end() )
      not_found = disabort ? false : true;
    else
      result = itr->second;
  }

  std::string preff = prefix;
  if ( !preff.empty() )
    preff+="::";
    
  
  if ( not_found )
  {
    DOMAIN_LOG_FATAL("object_registry::get: object '" << preff << name << "' not found" )
    return nullptr;
  }
   
  if ( !disabort && result == nullptr)
  {
    DOMAIN_LOG_FATAL("object_registry::get: invalid interface for " << preff << name <<
                     " (cannot convert from " << typeid(itr->second.get()).name() << " to " << typeid(result.get()).name() << ")" )
    return nullptr;
  }
  return result;
}

}
