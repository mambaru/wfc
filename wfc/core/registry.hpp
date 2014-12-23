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
  typedef std::mutex mutex_type;
  
public:
  
  typedef std::shared_ptr<iinterface> interface_ptr;
  typedef std::pair< std::string, std::string > key_type;
  typedef std::map< key_type, interface_ptr > registry_map;

  template<typename I>
  std::shared_ptr<I> get(const std::string& prefix, const std::string& name) const
  {
    std::shared_ptr<I> result = nullptr;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      auto itr = _registry_map.find( key_type(prefix, name) );
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
  
  template<typename I>
  std::shared_ptr<I> get(const std::string& name) const
  {
    return this->get<I>("", name);
  }


  void set(const std::string& prefix, const std::string& name, std::shared_ptr<iinterface> item )
  {
    if (name.empty() )
    {
      DAEMON_LOG_FATAL("wfc::registry::set: empty name " )
      abort();
    }
    std::lock_guard<mutex_type> lk(_mutex);
    _registry_map[ key_type(prefix, name)] = item;
  }
  
  void set(const std::string& name, std::shared_ptr<iinterface> item )
  {
    this->set("", name, item);
  }

  void erase(const std::string& prefix, const std::string& name)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _registry_map.erase( key_type(prefix,name) );
  }

  void erase(const std::string& name)
  {
    this->erase("", name);
  }

  
  template<typename I>
  void for_each( std::function< void(std::string, std::string, std::shared_ptr<I> ) > f )
  {
    registry_map rm;
    
    {
      std::lock_guard<mutex_type> lk(_mutex);
      rm = _registry_map;
    }

    for (const auto& a : rm)
    {
      std::shared_ptr<I> ptr = std::dynamic_pointer_cast<I>(a.second);
      if ( ptr != nullptr )
        f( a.first.first, a.first.second, ptr );
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
