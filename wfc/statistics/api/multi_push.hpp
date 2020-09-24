//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <wfc/statistics/api/push.hpp>
#include <deque>

namespace wfc{ namespace statistics{

namespace request
{
  struct multi_push
  {
    std::deque<push> data;
    typedef std::unique_ptr<multi_push> ptr;
  };
}

namespace response
{
  struct multi_push
  {
    // false - нет места для какого-то нового счетчика
    bool status = true;
    typedef std::unique_ptr<multi_push> ptr;
    typedef std::function< void(ptr)> handler;
  };
}

}}
