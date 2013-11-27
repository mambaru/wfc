#pragma once

#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <iostream>
#include <list>

namespace mamba{ namespace comet{

class callback_owner
{
public:
  
  callback_owner()
    : _alive( std::make_shared<int>(1) )
  {
  }

  /*
  std::function<bool(void)> callback(std::function<bool(void)> f)
  {
    std::weak_ptr<int> alive = _alive;
    return [alive,f]()-> bool
    {
      if ( auto p = alive.lock() )
      {
        return f();
      }
      return false;
    };
  }
  */
  template <typename T>
  struct identity
  {
    typedef T type;
  };

  //typename std::identity<std::function<bool(Args...)>>::type f
  template<typename ... Args>
  std::function<bool(Args...)> callback(typename identity<std::function<bool(Args...)>>::type f)
  {
    std::weak_ptr<int> alive = _alive;
    return [alive,f](Args... args)-> bool
    {
      if ( auto p = alive.lock() )
      {
        return f(args...);
      }
      return false;
    };
  }
 

  /*
  template<typename ... Args>
  std::function<bool(Args...)> callback_(std::function<bool(Args...)> f)
  {
    std::weak_ptr<int> alive = _alive;
    return [alive,f](Args... args)-> bool
    {
      if ( auto p = alive.lock() )
      {
        return f(args...);
      }
      return false;
    };
  }
  

  template<typename F>
  F callback(F f)
  {
    return callback_(f);
  }
  */
  
private:
  std::shared_ptr<int> _alive;
};


class callback_list
{
  typedef std::recursive_mutex mutex_type;
public:
  typedef std::function<bool(void)> callback_type;
  typedef std::list< callback_type > callback_set;

  void insert(callback_type f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _callback_set.push_back(f);
  }

  /*
  void erase(callback_type f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _callback_set.erase(f);
  }*/

  void for_each( )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _callback_set.begin();
    while ( itr!=_callback_set.end() )
    {
      if ( (*itr)() )
        ++itr;
      else
        _callback_set.erase(itr++);
    }
    //for (const auto& f : _callback_set)
    /*
    for (const auto& a : _registry_map)
      f( a.first, a.second );
    */
  }
  
private:
  callback_set _callback_set;
  mutable mutex_type _mutex;
};

}}
