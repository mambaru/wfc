#pragma once

#include <wfc/io/reader/async/tags.hpp>
#include <wfc/io/reader/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace async{

struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template gete<_on_read_>()( t, d->begin(), d->end() );
    t.get_aspect().template get<_incoming_>()( t, std::move(d) );
  }
};

}}}}
