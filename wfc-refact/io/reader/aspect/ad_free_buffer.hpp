#pragma once

namespace wfc{ namespace io{ namespace reader{ 

struct ad_free_buffer
{
  template<typename T>
  void operator()( T& , typename T::data_ptr )
  {
  }
};

}}}
