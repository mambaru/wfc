#pragma once
#include <wfc/inet/types.hpp>


namespace wfc{ namespace inet{ namespace conn{ namespace rn{

template<typename Tg, typename N>
struct ad_trimmer
{
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    if ( d->size() >= N::value )
      d->resize( d->size() - N::value );
    else
      d->clear();
    t.get_aspect().template get<Tg>()(t, std::move(d));
  }
};

}}}}
