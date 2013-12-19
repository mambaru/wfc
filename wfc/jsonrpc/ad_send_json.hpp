#pragma once

#include <wfc/jsonrpc/tags.hpp>
#include <wfc/jsonrpc/types.hpp>
#include <wfc/memory.hpp>
#include <atomic>
#include <iterator>

namespace wfc{ namespace jsonrpc{

class ad_send_json
{
public:
  ad_send_json()
    : _reserve(256)
  {
  }
  
  template<typename T,  typename J, typename V>
  void operator() (T& t, J,  const V& v)
  {
    data_ptr d = std::make_unique<data_type>();
    d->reserve(_reserve);

    typename J::serializer()(v, std::back_inserter(*d) );
    
    //const char *debug = &((*d)[0]);
    
    if (_reserve < d->size() )
      _reserve = d->size();
    if (_reserve > 8196 )
      _reserve = 8196;

    t.get_aspect().template get<_output_>()(t, std::move(d));
  }

private:
  std::atomic<size_t> _reserve;
};

}}
