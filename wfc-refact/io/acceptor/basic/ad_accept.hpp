#pragma once


#include <wfc/io/acceptor/basic/tags.hpp>

namespace wfc{ namespace io{ namespace acceptor{ namespace basic{

struct ad_accept
{
  template<typename T>
  void operator()(T& t)
  {
    typename T::data_ptr d = t.get_aspect().template get<_make_buffer_>()(t );
    
    if ( d != nullptr )
      t.get_aspect().template get< _accept_some_ >()(t, std::move(d) );

  }
};

}}}}
