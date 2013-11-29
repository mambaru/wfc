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
  template <typename T>
  struct identity { typedef T type; };

public:
  
  callback_owner()
    : _alive( std::make_shared<int>(1) )
  {
  }

  callback_owner(const callback_owner& ) = delete;
  callback_owner& operator = (const callback_owner& )  = delete;
  /*
  callback_owner(const callback_owner& )
    : _alive( std::make_shared<int>(1) )
  {
  }

  callback_owner& operator = (const callback_owner& ) 
  {
    _alive = std::make_shared<int>(1);
    return *this;
  }
  */

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
 
private:
  std::shared_ptr<int> _alive;
};



}}
