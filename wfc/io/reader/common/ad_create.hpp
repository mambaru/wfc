#pragma once

#include <wfc/io/reader/common/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace common{

struct ad_create
{
  template<typename T, typename Config>
  void operator()(T& t, const Config& config)
  {
    std::cout <<  "common ad_create {" << std::endl;
    t.get_aspect().template get<_input_buffer_size_>() = config.input_buffer_size;
    std::cout <<  "}common ad_create" << std::endl;

  }
};

}}}}
