#pragma once
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/types.hpp>

namespace wfc{ namespace btp{

namespace request
{
  struct add: wrtstat::aggregated_data
  {
    std::string name;
    typedef std::unique_ptr<add> ptr;
  };
}

namespace response
{
  struct add
  {
    // false - нет места для нового счетчика
    bool result = true;
    typedef std::unique_ptr<add> ptr;
    typedef std::function< void(ptr)> handler;
  };
}

}}
