//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <map>

namespace wfc{

template<typename>
class fire_list;


template<typename ...Args>
class fire_list< std::function<bool(Args...)> >
{
  typedef std::recursive_mutex mutex_type;
public:
  typedef std::function<bool(Args...)> fire_fun;
  typedef std::map< size_t, fire_fun > list_type;
  typedef size_t handler_id_t;

  handler_id_t insert( fire_fun f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _fire_list.insert( std::make_pair(_counter, f));
    return _counter++;
  }
  
  void erase(handler_id_t id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _fire_list.erase(id);
  }
  
  void clear()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _fire_list.clear();
  }

  void size() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _fire_list.size();
  }
  
  void fire(Args... args) const
  {
    list_type lst;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      lst = _fire_list;
    }
    
    auto beg = lst.begin();
    auto end = lst.end();
    for (;beg!=end;)
    {
      if ( beg->second(args...) )
      {
        ++beg;
      }
      else
      {
        lst.erase(beg++);
      }
    }
  }

private:
  size_t _counter = 1;
  list_type _fire_list;
  mutable mutex_type _mutex;
};

}
