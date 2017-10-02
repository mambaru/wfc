#pragma once
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/types.hpp>

namespace wfc{ namespace btp{

namespace request
{
  struct push: wrtstat::aggregated_data
  {
    std::string name;
    typedef std::unique_ptr<push> ptr;
  };
}

namespace response
{
  struct push
  {
    // false - нет места для нового счетчика
    bool result = true;
    typedef std::unique_ptr<push> ptr;
    typedef std::function< void(ptr)> handler;
  };
}

}}
