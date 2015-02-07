//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <list>


namespace wfc{

template<typename>
class fire_list;

template<typename ...Args>
class fire_list< std::function<void(Args...)> >
{
  typedef std::recursive_mutex mutex_type;
public:
  typedef std::function<void(Args...)> fire_fun;
  typedef std::list< fire_fun > list_type;

  void push_back( fire_fun f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _fire_list.push_back(f);
  }

  void push_front(fire_fun f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _fire_list.push_front(f);
  }

  /*
  void fire(std::function<void(fire_fun)> f)
  {
    list_type lst;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      lst = _fire_list;
    }
    for (const auto& f: lst)
      f(f);
  }
  */

  void fire(Args... args)
  {
    list_type lst;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      lst = _fire_list;
    }
    for (const auto& f: lst)
      f(args...);
    /*
    this->fire([this]() {
      
    });
    */
  }

private:
  
  list_type _fire_list;
  mutable mutex_type _mutex;
};

}
