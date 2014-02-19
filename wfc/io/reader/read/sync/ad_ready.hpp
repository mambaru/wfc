#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/reader/sync/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace sync{ 

struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "data size " << d->size() << std::endl;
    auto& lst = t.get_aspect().template get<_incoming_list_>();
    lst.push_back( std::move(d) );
  }

};

}}}}}
