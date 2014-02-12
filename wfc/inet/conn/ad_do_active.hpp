#pragma once

#include <wfc/inet/conn/tags.hpp>

namespace wfc{ namespace inet{

struct ad_do_active
{
  template<typename T, typename Itr>
  void operator()(T& t, Itr, Itr)
  {
    if ( auto activity = t.get_aspect().template get<_activity_>() )
      activity();
  }
};

}}
