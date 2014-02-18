#pragma once

namespace wfc{ namespace io{ namespace base{

struct ad_initialize
{
  template<typename T>
  void operator()(T& t, typename T::init_type& init)
  {
    typedef typename T::descriptor_type descriptor_type;
    t.get_aspect().template get<_descriptor_ptr_>() 
      = std::make_shared<descriptor_type>( *(init.io_service) );
  }
};

}}}
