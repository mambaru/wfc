#pragma once

#include <wfc/io/reader/aspect/tags.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_user_handler
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d)
  {
    auto handler = t.options().incoming_handler;
    if ( handler == nullptr )
    {
      t.get_aspect().template get<_output_>()(t, std::move(d) );
    }
    else
    {
      auto pthis = t.shared_from_this();
      auto callback = [pthis](typename T::data_ptr d)
      {
        typename T::lock_guard lk(pthis->mutex());
        if ( !pthis->status() )
          return;
        pthis->get_aspect().template get<_output_>()(*pthis, std::move(d) );
      };

      t.mutex().unlock();
      
      handler(
        std::move(d), 
        t.get_id(),
        callback
      );
      
      t.mutex().lock();
    }
  }
};


}}}