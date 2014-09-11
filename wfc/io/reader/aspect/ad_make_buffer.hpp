#pragma once

#include <wfc/io/reader/aspect/tags.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_make_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    return std::make_unique<typename T::data_type>( t.options().input_buffer_size );
  }
};

}}}