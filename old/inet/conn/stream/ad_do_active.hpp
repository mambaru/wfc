#pragma once

#include <wfc/inet/conn/tags.hpp>

namespace wfc{ namespace inet{ namespace conn{

struct ad_do_active
{
  template<typename T, typename Itr>
  void operator()(T& t, Itr, Itr)
  {
    if ( auto activity = t.get_aspect().template get<_activity_>() )
      activity();
  }
};

}}}
