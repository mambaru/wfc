#pragma once

#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace dgram{ namespace sync{
  
struct ad_init
{
  template<typename T>
  void operator()(T& t)
  {
    if ( t.options().outgoing_handler == nullptr )
    {
      std::weak_ptr<T> wthis = t.shared_from_this();
      
      t.options().outgoing_handler = 
        [wthis](typename T::data_ptr /*d*/)
        {
          if ( auto pthis = wthis.lock() )
          {
            typename T::lock_guard lk(pthis->mutex());
            if ( !pthis->status() )
              return;
            DAEMON_LOG_FATAL("::wfc::io::reader::dgram::sync: outgoing_handler not allowed for this")
            abort();
            // pthis->get_aspect().template get<_output_>()(*pthis, std::move(d) );
          }
        };
      
    }  
  }
};

}}}}}
