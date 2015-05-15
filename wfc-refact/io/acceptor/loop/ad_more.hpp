#pragma once

#include <wfc/io/acceptor/loop/tags.hpp>
#include <wfc/io/acceptor/basic/tags.hpp>

namespace wfc{ namespace io{ namespace acceptor{ namespace loop{

struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get< _accept_ >()(t );
  }
};

}}}}
