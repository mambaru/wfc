#pragma once

#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <iostream>

namespace wfc{

template<typename I>
class registry
{
  typedef std::recursive_mutex mutex_type;
  //typedef std::mutex mutex_type;
public:
  typedef std::map< std::string, std::weak_ptr<I> > registry_map;

  std::weak_ptr<I> get(const std::string& name) const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _registry_map.find(name);
    if ( itr == _registry_map.end() )
    {
      std::cout << (size_t)(this) << " registry not found " << name << " : " <<  _registry_map.size() << std::endl;
      return std::weak_ptr<I>(); 
    }
    return itr->second;
  }

  void set(const std::string& name, std::weak_ptr<I> item )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _registry_map[name] = item;
    std::cout  << (size_t)(this) << " registry set " << name << " : " <<  _registry_map.size() << " " << item.lock().get() <<  std::endl;
    /*auto itr = _registry_map.find(name);
    if ( itr != _registry_map.end() )
      _registry_map.erase(name);
    _registry_map.insert( std::make_pair(name, item) );
    return true;
    */
  }

  void erase(const std::string& name)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _registry_map.erase(name);
  }

  void for_each( std::function< void(std::string, std::weak_ptr<I> ) > f )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    for (const auto& a : _registry_map)
      f( a.first, a.second );
  }
  
private:
  registry_map _registry_map;
  mutable mutex_type _mutex;
};

}
