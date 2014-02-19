#pragma once

#include <wfc/io/basic/tags.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace basic{

struct ad_post
{
  template<typename T, typename Callback>
  void operator()(T& t, Callback callback)
  {
    t.get_io_service().post( 
      t.get_aspect().template get<_wrap_>()(t, callback) 
    );
  }
};

}}}
