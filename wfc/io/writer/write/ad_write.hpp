#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/writer/basic/tags.hpp>
#include <wfc/io/writer/write/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace write{

struct ad_write
{
  template<typename T>
  struct return_type
  {
    typedef size_t type;
  };
  
  template<typename T>
  size_t operator()(T& t, typename T::data_ptr d)
  {
    d = t.get_aspect().template get<basic::_make_buffer_>()(t, std::move(d) );
    
    if ( d != nullptr )
      t.get_aspect().template get< _outgoing_ >()(t, std::move(d) );
    
    size_t result = t.get_aspect().template get<_writed_size_>();
    t.get_aspect().template get<_writed_size_>() = 0;
    return result;
  }

};

}}}}
