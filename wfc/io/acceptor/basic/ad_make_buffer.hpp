#pragma once

#include <wfc/io/reader/common/tags.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace acceptor{ namespace basic{

// TODO: переименовать
struct ad_make_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    return std::make_unique<typename T::data_type>( t.get_io_service() );
  }
};

}}}}
