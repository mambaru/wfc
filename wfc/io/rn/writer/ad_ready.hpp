#pragma once

#include <wfc/io/rn/writer/tags.hpp>
#include <utility>

namespace wfc{ namespace io{ namespace rn{ namespace writer{

struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "writer ad_ready { " << t.get_id() << std::endl;
    t.get_aspect().template gete<_on_write_>()(t, d->begin(), d->end());
    std::cout << "writer ad_ready 1" << std::endl;
    t.get_aspect().template get<_outgoing_>()(t, std::move(d));
    std::cout << "} writer ad_ready " << std::endl;
  }
};

}}}}
