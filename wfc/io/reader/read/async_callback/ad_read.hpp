#pragma once

#include <wfc/io/reader/stream/async/tags.hpp>
#include <wfc/io/reader/read/async_callback/tags.hpp>
#include <wfc/io/reader/common/tags.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace reader{  namespace read{ namespace async_callback{

struct ad_read
{
  template<typename>
  struct return_type
  {
    typedef void type;
  };
  
  template<typename T, typename Callback>
  void operator()(T& t, Callback callback)
  {
    auto& lst = t.get_aspect().template get<_incoming_list_>();
    if ( lst.empty() )
    {
      auto d = t.get_aspect().template get<common::_make_buffer_>()(t);
      t.get_aspect().template get< _outgoing_ >()(t, std::move(d) );
    }
  
    if ( !lst.empty() )
    {
      typename T::data_ptr d = std::move( lst.front() );
      lst.pop_front();
      callback( std::move(d) );
    }
    t.get_aspect().template get<_callback_list_>().push_back(callback);

  }
};

}}}}}
