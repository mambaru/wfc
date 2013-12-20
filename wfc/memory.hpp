#pragma once

#include <memory>
#include <utility>

namespace std{

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
  return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

}


namespace wfc{
  
template<typename T>
class unique_wrapper
{
public:
  unique_wrapper(std::unique_ptr<T> c)
    : _ptr( std::move(c) ) 
  {
    
  };
  
  unique_wrapper(const unique_wrapper& uc)
    : _ptr(std::move( const_cast<unique_wrapper&>(uc)._ptr)) 
  {
  };
  
  unique_wrapper& operator=(const unique_wrapper& uc) = delete;
  
  std::unique_ptr<T> get() 
  {
    return std::move(_ptr);
  };
  
private:
  std::unique_ptr<T> _ptr;
};

template<typename T>
unique_wrapper<T> unique_wrap(std::unique_ptr<T> ptr)
{
  return unique_wrapper<T>( std::move(ptr) );
}

template<typename T>
std::unique_ptr<T> unique_unwrap(unique_wrapper<T> wrp)
{
  return std::move( wrp.get() );
}

}