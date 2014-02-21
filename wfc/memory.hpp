#pragma once

#include <memory>
#include <utility>
#include <iostream>
#include <string>
namespace std{


template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
  return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

}


namespace wfc{
  /*
template<typename T>
class unique_wrapper
{
public:
  unique_wrapper(std::unique_ptr<T> c,  std::string tmp="")
    : _ptr( new std::unique_ptr<T>(std::move(c)) ) 
    , tmp(tmp)
  {
    
  };
  
  unique_wrapper(const unique_wrapper& uc)
    : //_ptr(std::move( const_cast<unique_wrapper&>(uc)._ptr))
    _ptr( new std::unique_ptr<T>(const_cast<unique_wrapper&>(uc)._ptr) )
    , tmp(uc.tmp)
  {
    std::cout << "unique_wrapper(const unique_wrapper& uc) " << (_ptr==nullptr) <<  " tmp=" << tmp<<  std::endl;
  };
  
  unique_wrapper& operator=(const unique_wrapper& uc) = delete;
  
  std::unique_ptr<T> get() 
  {
    return std::move(*_ptr);
  };
  
private:
  std::unique_ptr<T>* _ptr;
  std::string tmp;
};

template<typename T>
unique_wrapper<T> unique_wrap(std::unique_ptr<T> ptr,  std::string tmp="")
{
  return unique_wrapper<T>( std::move(ptr), tmp);
}

template<typename T>
std::unique_ptr<T> unique_unwrap(unique_wrapper<T> wrp)
{
  return std::move( wrp.get() );
}
*/
}