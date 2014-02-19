#pragma once

#include <wfc/io/reader/common/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace common{

struct ad_create
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_input_buffer_size_>() = 1024*8;
  }
};

}}}}
