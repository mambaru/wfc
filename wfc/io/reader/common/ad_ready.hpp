#pragma once

#include <wfc/io/reader/tags.hpp>
#include <wfc/io/reader/common/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace common{

struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "common ad_ready { " << std::endl;
    t.get_aspect().template gete<_on_read_>()( t, d->begin(), d->end() );
    t.get_aspect().template get<_outgoing_>()( t, std::move(d) );
    std::cout << "} common ad_ready " << std::endl;
  }
};

}}}}
