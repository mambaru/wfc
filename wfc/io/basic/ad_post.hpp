#pragma once

#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace basic{

struct ad_post
{
  template<typename T, typename Callback>
  void operator()(T& t, Callback callback)
  {
    t.strand().post( t.owner().wrap(callback) );
    /*
    t.get_io_service().post( 
      t.strand().wrap(callback)
      //t.get_aspect().template get<_wrap_>()(t, callback) 
    );
    */
  }
};

}}}
