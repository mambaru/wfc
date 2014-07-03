#pragma once

#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_init
{
  template<typename T>
  void operator()(T& t)
  {
    if ( t.options().outgoing_handler == nullptr )
    {
      std::weak_ptr<T> wthis = t.shared_from_this();
      t.options().outgoing_handler = [wthis](typename T::data_ptr d)
      {
        if ( auto pthis = wthis.lock() )
        {
          typename T::lock_guard lk(pthis->mutex());
          if ( !pthis->status() )
            return;
          
          pthis->get_aspect().template get<_output_>()(*pthis, std::move(d) );
        }
      };
    }  }
};

struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {

    
    //t.dispatch( [&t]() {
    t.get_aspect().template get<_read_more_>()(t);
    //});
  }
};

}}}
