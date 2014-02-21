#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/reader/stream/sync/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace sync{ 

struct ad_incoming
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto& lst = t.get_aspect().template get<_incoming_list_>();
    lst.push_back( std::move(d) );
  }

};

}}}}}
