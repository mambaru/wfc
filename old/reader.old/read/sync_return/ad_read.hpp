#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/reader/common/tags.hpp>
#include <wfc/io/reader/read/sync_return/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace sync_return{ 

struct ad_read
{
  template<typename T>
  struct return_type
  {
    typedef typename T::data_ptr type;
  };
  
  template<typename T>
  typename T::data_ptr operator()(T& t)
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
      return std::move(d);
    }
    
    std::cout << "list empty" << std::endl;
    
    return nullptr;
  }

};

}}}}}
