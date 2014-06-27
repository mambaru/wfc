#pragma once

#include <wfc/io/reader/aspect/tags.hpp>

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
      /*
      typedef std::chrono::high_resolution_clock clock_t;
      clock_t::time_point start = clock_t::now();
      */
      auto callback =  t.callback([&t/*, start*/](typename T::data_ptr d)
        {
          typename T::lock_guard lk(t.mutex());
          t.get_aspect().template get<_output_>()(t, std::move(d) );
        });

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