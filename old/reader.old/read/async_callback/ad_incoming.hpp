#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/reader/read/async_callback/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace async_callback{ 

struct ad_incoming
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto& clb_lst = t.get_aspect().template get<_callback_list_>();
    if ( !clb_lst.empty() )
    {
      auto callback = clb_lst.front();
      clb_lst.pop_front();
      callback( std::move(d));
    }
    else
    {
      auto& lst = t.get_aspect().template get<_incoming_list_>();
      lst.push_back( std::move(d) );
    }
  }
};

}}}}}
