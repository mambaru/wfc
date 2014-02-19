#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace reader{

struct ad_buffer_manager
{
  template<typename T>
  typename T::data_type& create(T& t)
  {
    auto d = t.get_aspect().template get<_create_buffer_>()(t);
    t.get_aspect().template get<_input_list_>().push( std::move(d) );
    return *(t.get_aspect().template get<_input_list_>().back());
  }
  
  template<typename T>
  typename T::data_ptr pop(T& t)
  {
    auto d = std::move( t.get_aspect().template get<_input_list_>().front() );
    t.get_aspect().template get<_input_list_>().pop();
    return d;
  }
};

}}}
