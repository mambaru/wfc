#pragma once

#include <wfc/io/writer/basic/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace basic{

struct ad_create
{
  template<typename T, typename Config>
  void operator()(T& t, const Config& config)
  {
    t.get_aspect().template get<_output_buffer_size_>() = config.output_buffer_size;
  }
};

}}}}
