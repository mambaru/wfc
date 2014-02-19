#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/reader/common/tags.hpp>
#include <wfc/io/reader/sync/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace sync{ 

struct ad_read
{
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    auto& lst = t.get_aspect().template get<_incoming_list_>();
    if ( lst.empty() )
    {
      // auto d = std::make_unique<typename T::data_type>(8096);
      auto d = t.get_aspect().template get<common::_make_buffer_>()(t);
      t.get_aspect().template get< wfc::io::reader::sync::_read_some_ >()(t, std::move(d) );
    }
  
    std::cout << "list size " << lst.size() << std::endl;
    if ( !lst.empty() )
    {
      typename T::data_ptr d = std::move( lst.front() );
      lst.pop_front();
      return std::move(d);
    }
    std::cout << "list empty" << std::endl;
    return nullptr;
  }

};

}}}}}
