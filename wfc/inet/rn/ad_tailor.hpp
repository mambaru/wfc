#pragma once

#include <wfc/inet/types.hpp>

namespace wfc{ namespace inet{ namespace rn{

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
    std::cout << "WRITE[[" << std::string(d->begin(), d->end() ) << "]]" << std::endl;
    t.get_aspect().template get<Tg>()( t, std::move(d) );
  }

private:
  const char *beg;
  const char *end;
  data_ptr _data;
};

}}}
