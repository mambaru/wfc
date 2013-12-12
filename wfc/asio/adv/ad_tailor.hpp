#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <wfc/callback/callback_owner.hpp>
#include <wfc/asio/iconnection.hpp>

namespace wfc{ namespace inet{

template<typename Tg, typename Str /*= rn*/>
struct ad_tailor
{
  ad_tailor()
    : beg(Str()())
    , end(beg + std::strlen(beg))
  {
  }
  
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    d->reserve( d->size() + std::distance(beg, end) );
    std::copy( beg, end, std::inserter(*d, d->end()) );
    t.get_aspect().template get<Tg>()( t, std::move(d) );
  }

private:
  const char *beg;
  const char *end;
  data_ptr _data;
};

}}
