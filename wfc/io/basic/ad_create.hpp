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

    typedef typename T::aspect::template advice_cast<_strand_type_>::type strand_type;
    t.get_aspect().template get<_strand_>() 
      = std::make_shared<strand_type>( t.get_io_service() );
      
    t.get_aspect().template get<_not_alive_>() = init.not_alive;
  }
};

}}}
