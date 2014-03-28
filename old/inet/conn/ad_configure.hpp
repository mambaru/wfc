#pragma once

#include <wfc/inet/conn/tags.hpp>

namespace wfc{ namespace inet{ namespace conn{

struct ad_configure
{
  template<typename T, typename Conf>
  void operator()( T& t, Conf conf)
  {
    t.get_aspect().template get<_context_>().enable_stat = conf.enable_stat;
  }
};

}}}
