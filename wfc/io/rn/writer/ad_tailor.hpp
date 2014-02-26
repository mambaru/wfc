#pragma once

#include <wfc/inet/types.hpp>
#include <cstring>

namespace wfc{ namespace io{ namespace rn{ namespace writer{

struct ad_tailor
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    static const char* sep =  t.get_aspect().template get<_separator_>()();
    static size_t size = std::strlen( sep );
    d->reserve( d->size() + size );
    std::copy( sep, sep + size, std::inserter(*d, d->end()) );
    t.get_aspect().template get<_ready_>()( t, std::move(d) );
  }
private:
};

}}}}
