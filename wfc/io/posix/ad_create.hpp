#pragma once

#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace posix{

struct ad_create
{
  template<typename T, typename Init>
  void operator()(T& t, const Init& init)
  {
    t.get_aspect().template get<_descriptor_ptr_>()->assign( init.native_handle );
  }
};

}}}
