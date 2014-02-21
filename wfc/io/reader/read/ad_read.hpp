#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/reader/common/tags.hpp>
#include <wfc/io/reader/stream/sync/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace read{

struct ad_read
{
  template<typename T>
  struct return_type
  {
    typedef void type;
  };
  
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    auto d = t.get_aspect().template get<common::_make_buffer_>()(t);
    t.get_aspect().template get< wfc::io::reader::_incoming_>()(t, std::move(d) );
    
  }

};

}}}}
