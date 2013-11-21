#pragma once

#include <unordered_map>
#include <memory>
#include <string>

namespace mamba{ namespace comet{

template<typename I>
class registry
{
public:
  typedef std::unordered_map< std::string, std::weak_ptr<I> > registry_map;

  std::weak_ptr<I> find(const std::string& name) const
  {
    auto itr = _registry_map.find(name);
    if ( itr == _registry_map.end() )
      return std::weak_ptr<I>();
    return itr->second;
  }

  bool insert(const std::string& name, std::weak_ptr<I> item )
  {
    auto itr = _registry_map.find(name);
    if ( itr != _registry_map.end() )
      return false;
    _registry_map.insert( std::make_pair(name, item) );
    return true;
  }

  void erase(const std::string& name)
  {
    _registry_map.erase(name);
  }

  void for_each( std::function< void(std::string, std::weak_ptr<I> ) > f )
  {
    for (const auto& a : _registry_map)
      f( a.first, a.second );
  }
  
private:
  
  registry_map _registry_map;
};

}}
