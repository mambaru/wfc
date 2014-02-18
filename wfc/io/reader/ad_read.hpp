#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace reader{

struct ad_read
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template get<_incoming_list_>().push( std::move(d) );
  }
  
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    while( t.get_aspect().template get<_incoming_list_>().empty() )
    {
      t.get_aspect().template get<_read_some_>()(t);
    }
  
    typename T::data_ptr d = std::move( t.get_aspect().template get<_incoming_list_>().front() );
    t.get_aspect().template get<_incoming_list_>().pop();
    return std::move(d);
  }

  
private:
  
  
};

}}}
