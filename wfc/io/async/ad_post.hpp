#pragma once

namespace wfc{ namespace io{ namespace async{

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
