#pragma once

#include <string>
#include <memory>
#include <functional>

namespace wfc{ namespace statistics{

namespace request
{
  struct del
  {
    std::string name;
    typedef std::unique_ptr<del> ptr;
  };
}

namespace response
{
  struct del 
  {
    // false - если не найден
    bool status = false;
    typedef std::unique_ptr<del> ptr;
    typedef std::function<void(ptr)> handler;
  };
}

}}
