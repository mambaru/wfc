#pragma once

namespace wfc{ namespace io{ namespace async{

struct ad_dispatch
{
  template<typename T, typename Callback>
  void operator()(T& t, Callback callback)
  {
    t.get_io_service().dispatch( 
      t.get_aspect().template get<_wrap_>()(t, callback) 
    );
  }
};

}}}
