#pragma once

#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace basic{

struct ad_dispatch
{
  template<typename T, typename Callback>
  void operator()(T& t, Callback callback)
  {
    std::cout <<  "t.get_io_service().dispatch" <<  std::endl;
    t.get_io_service().dispatch( 
      t.strand().wrap(callback)
    );
  }
};

}}}
