#pragma once

#include <wfc/io/writer/tags.hpp>
#include <wfc/io/writer/basic/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace basic{

struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, std::size_t bytes_transferred)
  {
    t.get_aspect().template gete<_on_write_>()( t, d->begin(), d->begin() + bytes_transferred );
    t.get_aspect().template get<_process_>()( t, std::move(d), bytes_transferred);
  }
};

}}}}
