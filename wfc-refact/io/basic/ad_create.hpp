#pragma once

#include <wfc/io/basic/tags.hpp>
#include <wfc/io/tags.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace basic{

struct ad_create
{
  template<typename T/*,  typename Init*/>
  void operator()(T& t/*, const Init&*/ )
  {
    typename T::lock_guard lk(t.mutex());
    
    typedef typename T::aspect::template advice_cast<_owner_type_>::type owner_type;
    t.get_aspect().template get<_owner_>() = std::make_shared<owner_type>();

    typedef typename T::aspect::template advice_cast<_strand_type_>::type strand_type;
    t.get_aspect().template get<_strand_>() 
      = std::make_shared<strand_type>( t.get_io_service() );
    
    // Копируем, т.к. на момент вызова not_alive объект t уже умер
    t.get_aspect().template get<_not_alive_>() = t.options().not_alive;
    //t.get_aspect().template get<_transfer_handler_>() = t.options().outgoing_handler;
    
    
  }
};

}}}
