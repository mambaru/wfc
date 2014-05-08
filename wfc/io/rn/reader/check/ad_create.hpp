#pragma once

#include <wfc/io/rn/reader/check/tags.hpp>


namespace wfc{ namespace io{ namespace rn{ namespace reader{ namespace check{

struct ad_create
{
  template<typename T, typename Init>
  void operator()(T& t, const Init& init)
  {
    t.get_aspect().template get< _limit_error_ >() = init.rn_limit_error;
    t.get_aspect().template get< _limit_warning_ >() = init.rn_limit_warning;
  }
};

}}}}}
