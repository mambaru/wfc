#pragma once
#include <wfc/inet/tags.hpp>
#include <fas/aop/tag.hpp>


namespace wfc{ namespace inet{

template<typename Tg>
struct ad_connection_start
{
  template<typename T>
  void operator()(T& t, fas::tag<_start_>)
  {
    t.get_aspect().template get<Tg>()(t);
  }
};

}}
