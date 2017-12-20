//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/logger.hpp>

#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>

namespace wfc{

class object_registry
{
  typedef std::mutex mutex_type;
  
public:
  
  typedef std::shared_ptr<iinterface> interface_ptr;
  typedef std::pair< std::string, std::string > key_type;
  typedef std::map< key_type, interface_ptr > registry_map;
  
  template<typename I>
  std::shared_ptr<I> get(const std::string& prefix, const std::string& name, bool disabort = false) const
  {
    if ( name.empty() )
      return nullptr;

    bool not_found = false;
    std::shared_ptr<I> result = nullptr;
    registry_map::const_iterator itr;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      itr = _registry_map.find( key_type(prefix, name) );
      if ( itr == _registry_map.end() )
        not_found = disabort ? false : true;
      else
        result = std::dynamic_pointer_cast<I>(itr->second);
    }

    if ( not_found )
    {
      DOMAIN_LOG_FATAL("wfc::registry::get: object '" << prefix << "::" << name << "' not found" )
      return nullptr;
    }
    
    if ( !disabort && result == nullptr)
    {
      DOMAIN_LOG_FATAL("wfc::registry::get: invalid interface for " << prefix << "::" << name <<
                       " (cannot convert from " << typeid(itr->second.get()).name() << " to " << typeid(result.get()).name() << ")" )
      return nullptr;
    }

    return result;
  }
  
  template<typename I>
  std::shared_ptr<I> get(const std::string& name, bool disabort = false) const
  {
    return this->get<I>("", name, disabort);
  }

  template<typename I>
  std::map<std::string, std::shared_ptr<I> > select(const std::string& prefix) const
  {
    std::map<std::string, std::shared_ptr<I> > mp;
    this->for_each<I>(prefix, [&mp](std::string name, std::shared_ptr<I> p){
      mp[name] = p;
    });
    return mp;
  }
  
  void set(const std::string& prefix, const std::string& name, std::shared_ptr<iinterface> item, bool nomark = false )
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
  
  void set(const std::string& name, std::shared_ptr<iinterface> item, bool nomark = false )
  {
    this->set("", name, item, nomark);
  }

  void erase(const std::string& prefix, const std::string& name)
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

  void erase(const std::string& name)
  {
    this->erase("", name);
  }
  
  template<typename I>
  void for_each( std::function< void(std::string, std::string, std::shared_ptr<I> ) > f ) const
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

  template<typename I>
  void for_each( 
    std::string prefix,
    std::function< void( std::string, std::shared_ptr<I> ) > f,
    std::function< bool( std::shared_ptr<I>, std::shared_ptr<I> ) > cmp = nullptr) const
  {
    typedef std::pair< std::string, std::shared_ptr<I> > value_type;
    std::vector< value_type > rm;
    
    {
      std::lock_guard<mutex_type> lk(_mutex);
      rm.reserve(_registry_map.size());
      auto beg = _registry_map.lower_bound( key_type(prefix, "")  );
      auto end = _registry_map.lower_bound( key_type(prefix+" ", "")  );
      for (;beg!=end; ++beg)
      {
        if (std::shared_ptr<I> ptr = std::dynamic_pointer_cast<I>( beg->second) )
          rm.push_back( std::make_pair( beg->first.second, ptr ) );
      };
    }
    
    if ( cmp != nullptr )
    {
      std::sort(
        rm.begin(), 
        rm.end(), 
        [cmp](value_type left, value_type right)->bool
        {
          return cmp(left.second, right.second);
        } 
      );
    }

    for (const auto& a : rm)
      f( a.first, a.second );

  }

  void clear()
  {
    registry_map rm;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _registry_map.swap(rm);
      ++_dirty;
    }
    rm.clear();
  }
  
  size_t size() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _registry_map.size();
  }

  int dirty() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _dirty;
  }
  
  int reset_dirty()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    int result = _dirty;
    _dirty = 0;
    return result;
  }
  

private:
  registry_map _registry_map;
  mutable mutex_type _mutex;
  int _dirty = 0;
};

}
