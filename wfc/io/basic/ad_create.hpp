#pragma once

#include <wfc/io/basic/tags.hpp>
#include <wfc/io/tags.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace basic{

struct ad_create
{
  template<typename T,  typename Init>
  void operator()(T& t, const Init& init)
  {
    typedef typename T::aspect::template advice_cast<_owner_type_>::type owner_type;
    t.get_aspect().template get<_owner_>() = std::make_shared<owner_type>();
    
    t.get_aspect().template get<_io_service_ptr_>() = init.io_service;

    typedef typename T::aspect::template advice_cast<_descriptor_type_>::type descriptor_type;
    t.get_aspect().template get<_descriptor_ptr_>() 
      = std::make_shared<descriptor_type>( *init.io_service );

    typedef typename T::aspect::template advice_cast<_strand_type_>::type strand_type;

    t.get_aspect().template get<_strand_>() 
      = std::make_shared<strand_type>( *init.io_service );
      
    // TODO: перенести в log
    t.get_aspect().template get<_not_alive_>() = init.not_alive;


  }
};

}}}
