#pragma once

#include <wfc/callback/callback_status.hpp>
#include <wfc/logger.hpp>
//#include <map>
#include <memory>
//#include <string>
//#include <mutex>
#include <iostream>
#include <list>

namespace wfc{
  
template<typename H, typename NA >
struct callback_wrapper
{
  H _handler;
  NA _not_alive;
  typedef std::weak_ptr<int> weak_type;
  
  callback_wrapper(H h, NA not_alive,  weak_type alive)
    : _handler( h )
    , _not_alive(not_alive )
    , _alive(alive)
  {
  }
  
  template <class... Args>
  auto operator()(Args&&... args)
    -> decltype( _handler(std::forward<Args>(args)...)) 
  {
    if ( auto p = _alive.lock() )
    {
      return _handler(std::forward<Args>(args)...);
    }
    
    return _not_alive();
  }
private:
  weak_type _alive;
};

template<typename H>
struct callback_wrapper2
{
  H _handler;
  typedef std::weak_ptr<int> weak_type;
  
  callback_wrapper2(H h, weak_type alive)
    : _handler( h )
    , _alive(alive)
  {
  }
  
  template <class... Args>
  auto operator()(Args&&... args)
    -> decltype( _handler(std::forward<Args>(args)...)) 
  {
    if ( auto p = _alive.lock() )
    {
      return _handler(std::forward<Args>(args)...);
    }
    return decltype( _handler(std::forward<Args>(args)...))();
  }
private:
  weak_type _alive;
};



// TODO: spinlock
// а лучше callback_owner<mutex_type>
//template<typename M = std::mutex >
class callback_owner
{
  template <typename T>
  struct identity { typedef T type; };

public:

  //typedef M mutex_type;
  
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

  //mutex_type& mutex() {return _mutex;}
  
  void reset()
  {
    //std::lock_guard<mutex_type> lk(_mutex);
    _alive = std::make_shared<int>(1);
  }
  
  /*
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
  */

  
  template<typename R, typename ... Args>
  std::function<R(Args&&...)> callback(typename identity<std::function<R(Args&&...)>>::type f)
  {
    std::weak_ptr<int> alive = _alive;

    return [alive,f](Args&&... args) -> R
    {
      if ( auto p = alive.lock() )
      {
        return f(std::forward<Args>(args)...);
      }
      return R();
    };
  }

  template<typename Handler, typename NotAliveHandler>
  callback_wrapper<Handler, NotAliveHandler>
  wrap(Handler handler, NotAliveHandler not_alive)
  {
    return callback_wrapper<Handler, NotAliveHandler>( handler, not_alive, _alive);
  }

  template<typename Handler>
  callback_wrapper2<Handler>
  wrap(Handler handler)
  {
    return callback_wrapper2<Handler>( handler, _alive);
  }

  /*
  template<typename Strand, typename Handler, typename NotAliveHandler = std::function<void()> >
  callback_wrapper<Handler, NotAliveHandler>
  wrap(Strand& strand, Handler handler, NotAliveHandler not_alive = nullptr)
  {
    Handler wrp = strand.wrap(handler);
    return this->wrap( wrp, not_alive );
  }
  */

  /*
  template<typename Strand, typename Handler, typename NotAliveHandler>
  callback_wrapper<Handler, NotAliveHandler>
  wrap(Strand& strand, Handler handler, NotAliveHandler not_alive)
  {
    return callback_wrapper<Handler, NotAliveHandler>( strand, handler, not_alive, _alive);
  }
  */

private:
  
  alive_type _alive;
};
/*
  template<typename Handler, typename NotAliveHandler>
  inline
  callback_wrapper<Handler, NotAliveHandler>
  owner_wrap(callback_owner& owner, Handler handler, NotAliveHandler not_alive)
  {
    return owner.wrap( handler, not_alive);
  }

*/
}
