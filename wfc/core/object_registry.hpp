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
  std::shared_ptr<I> get(const std::string& prefix, const std::string& name, bool disabort = false) const;
  
  template<typename I>
  std::shared_ptr<I> get(const std::string& name, bool disabort = false) const;

  template<typename I>
  std::map<std::string, std::shared_ptr<I> > select(const std::string& prefix) const;
  
  void set(const std::string& prefix, const std::string& name, std::shared_ptr<iinterface> item, bool nomark = false );
  
  void set(const std::string& name, std::shared_ptr<iinterface> item, bool nomark = false );

  void erase(const std::string& prefix, const std::string& name);

  void erase(const std::string& name);
  
  template<typename I>
  void for_each( std::function< void(std::string, std::string, std::shared_ptr<I> ) > f ) const;

  template<typename I>
  void for_each( 
    std::string prefix,
    std::function< void( std::string, std::shared_ptr<I> ) > f,
    std::function< bool( std::shared_ptr<I>, std::shared_ptr<I> ) > cmp = nullptr) const;

  void clear();
  
  size_t size() const;

  int dirty() const;
  
  int reset_dirty();

private:
  
  std::shared_ptr<iinterface> get_(const std::string& prefix, const std::string& name, bool disabort) const;
  
private:
  registry_map _registry_map;
  mutable mutex_type _mutex;
  int _dirty = 0;
};


template<typename I>
std::shared_ptr<I> object_registry::get(const std::string& prefix, const std::string& name, bool disabort ) const
{
  std::shared_ptr<I> result = nullptr;
  if ( auto p = this->get_(prefix, name, disabort) )
    result = std::dynamic_pointer_cast<I>(p);
  return result;
}

template<typename I>
std::shared_ptr<I> object_registry::get(const std::string& name, bool disabort) const
{
  return this->get<I>("", name, disabort);
}

template<typename I>
std::map<std::string, std::shared_ptr<I> >
  object_registry::select(const std::string& prefix) const
{
  std::map<std::string, std::shared_ptr<I> > mp;
  this->for_each<I>(prefix, [&mp](std::string name, std::shared_ptr<I> p){
    mp[name] = p;
  });
  return mp;
}

template<typename I>
void object_registry::for_each( std::function< void(std::string, std::string, std::shared_ptr<I> ) > f ) const
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
void object_registry::for_each( 
  std::string prefix,
  std::function< void( std::string, std::shared_ptr<I> ) > f,
  std::function< bool( std::shared_ptr<I>, std::shared_ptr<I> ) > cmp) const
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


}
