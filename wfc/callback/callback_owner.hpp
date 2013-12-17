#pragma once

#include <wfc/callback/callback_status.hpp>
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <iostream>
#include <list>

namespace wfc{

// TODO: spinlock
// а лучше callback_owner<mutex_type>
template<typename M = std::mutex >
class callback_owner
{
  template <typename T>
  struct identity { typedef T type; };

public:

  typedef M mutex_type;
  
  typedef std::shared_ptr<int> alive_type;
  typedef std::weak_ptr<int>   weak_type;
  
  callback_owner()
    : _alive( std::make_shared<int>(1) )
  {
  }

  callback_owner(const callback_owner& ) = delete;
  callback_owner& operator = (const callback_owner& ) = delete;

  alive_type& alive() {return _alive;}
  const alive_type& alive() const {return _alive;}

  mutex_type& mutex() {return _mutex;}
  
  void reset()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _alive = std::make_shared<int>(1);
  }
  
  template<typename ... Args>
  std::function<callback_status(Args&&...)> callback(typename identity<std::function<callback_status(Args&&...)>>::type f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    std::weak_ptr<int> alive = _alive;
    _mutex.unlock();
    
    return [alive,f](Args&&... args)-> callback_status
    {
      if ( auto p = alive.lock() )
      {
        return f(std::forward<Args>(args)...);
      }
      return callback_status::died;
    };
  }
 
private:
  
  alive_type _alive;
  mutex_type _mutex;
  
};



}
