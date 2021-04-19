//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wrtstat/aggregator/api/aggregated_data.hpp>
#include <wrtstat/aggregator/api/types.hpp>

namespace wfc{ namespace statistics{

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
    bool status = true;
    typedef std::unique_ptr<push> ptr;
    typedef std::function< void(ptr)> handler;
  };
}

}}
