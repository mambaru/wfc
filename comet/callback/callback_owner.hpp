#pragma once

#include <comet/callback/callback_status.hpp>
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
  std::function<callback_status(Args...)> callback(typename identity<std::function<callback_status(Args...)>>::type f)
  {
    std::weak_ptr<int> alive = _alive;
    return [alive,f](Args... args)-> callback_status
    {
      if ( auto p = alive.lock() )
      {
        return f(args...);
      }
      return callback_status::died;
    };
  }
 
private:
  alive_type _alive;
};



}}
