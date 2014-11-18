#pragma once

#include <wfc/callback/callback_status.hpp>
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <iostream>
#include <list>


namespace wfc{

template<typename ...Args>
class callback_list_timed
{
  // Базовый класс
};

template<typename ...Args>
class callback_list_timelife
{
  // Ограниченное время жизни
};

template<typename ...Args>
class callback_list_timewait
{
  // Ограниченное время ожидания,  полсе чего удаляется
  
};

template<typename>
class callback_list;


template<typename ...Args>
class callback_list< std::function<callback_status(Args...)> >
{
  typedef std::recursive_mutex mutex_type;
public:
  typedef std::function<callback_status(Args...)> callback_type;
  typedef std::list< callback_type > list_type;

  void push_back(callback_type f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _callback_list.push_back(f);
  }

  void push_front(callback_type f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _callback_list.push_front(f);
  }

  void fire(std::function<callback_status(callback_type)> fire_fun)
  {
    _mutex.lock();
    auto itr = _callback_list.begin();
    while ( itr!=_callback_list.end() )
    {
      if ( fire_fun(*itr) != callback_status::died )
        ++itr;
      else
        _callback_list.erase(itr++);
    }
  }
  /*
  void fire(Args... args )
  {
    _mutex.lock();
    auto itr = _callback_list.begin();
    while ( itr!=_callback_list.end() )
    {
      if ( (*itr)(args...) != callback_status::died )
        ++itr;
      else
        _callback_list.erase(itr++);
    }
  }
  */
private:
  list_type _callback_list;
  mutable mutex_type _mutex;
};
/*
template<typename ...Args>
class callback_list
{
  typedef std::recursive_mutex mutex_type;
public:
  typedef std::function<bool(Args...)> callback_type;
  typedef std::list< callback_type > list_type;

  void push_back(callback_type f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _callback_list.push_back(f);
  }

  void push_front(callback_type f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _callback_list.push_front(f);
  }

  void fire(Args... args )
  {
    _mutex.lock();
    auto itr = _callback_list.begin();
    while ( itr!=_callback_list.end() )
    {
      if ( (*itr)(args...) )
        ++itr;
      else
        _callback_list.erase(itr++);
    }
  }
private:
  list_type _callback_list;
  mutable mutex_type _mutex;
};
*/


}
