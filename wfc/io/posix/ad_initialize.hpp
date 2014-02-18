#pragma once

namespace wfc{ namespace io{ namespace posix{

struct ad_initialize
{
  template<typename T>
  void operator()(T& t, typename T::init_type& init)
  {
    t.get_aspect().template get<_descriptor_ptr_>()->assign( init.native_handle );
  }
};

}}}
