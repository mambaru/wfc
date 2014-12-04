#pragma once

#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <iostream>

#include <wfc/core/iinterface.hpp>
#include <wfc/logger.hpp>

namespace wfc{

class interface_registry
{
  typedef std::recursive_mutex mutex_type;
public:
  typedef std::shared_ptr<iinterface> interface_ptr;
  typedef std::map< std::string, interface_ptr > registry_map;

  template<typename I>
  std::shared_ptr<I> get(const std::string& name) const
  {
    std::shared_ptr<I> result = nullptr;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      auto itr = _registry_map.find(name);
      if ( itr == _registry_map.end() )
      {
        return result; 
      }
      result = std::dynamic_pointer_cast<I>(itr->second);
    }
    if (result == nullptr)
    {
      DAEMON_LOG_FATAL("wfc::registry::get: invalid interface for " << name )
      abort();
    }
    
    return result;
  }

  void set(const std::string& name, std::shared_ptr<iinterface> item )
  {
    if (name.empty() )
    {
      DAEMON_LOG_FATAL("wfc::registry::set: empty name " )
      abort();
    }
    std::lock_guard<mutex_type> lk(_mutex);
    _registry_map[name] = item;
  }

  void erase(const std::string& name)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _registry_map.erase(name);
  }

  template<typename I>
  void for_each( std::function< void(std::string, std::shared_ptr<I> ) > f )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    for (const auto& a : _registry_map)
    {
      std::shared_ptr<I> ptr = std::dynamic_pointer_cast<I>(a.second);
      if ( ptr != nullptr )
        f( a.first, ptr );
    }
  }
  
  void clear()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _registry_map.clear();
  }
  
private:
  registry_map _registry_map;
  mutable mutex_type _mutex;
};

}
