#pragma once
#include <wfc/io/writer/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace writer{

struct ad_write_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, std::size_t bytes_transferred)
  {
    t.get_aspect().template get<_outgoing_buffer_size_>()-=bytes_transferred;
    t.get_aspect().template gete<_on_write_>()( t, d->begin(), d->begin() + bytes_transferred );
    t.get_aspect().template get<_preapare_buffer_>()( t, std::move(d), bytes_transferred);
    t.get_aspect().template get<_write_more_>()( t );
  }
};


}}}
