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
