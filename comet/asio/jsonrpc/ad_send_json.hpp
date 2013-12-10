#pragma once

#include <comet/asio/jsonrpc/tags.hpp>
#include <comet/asio/types.hpp>
#include <comet/memory.hpp>
#include <atomic>
#include <iterator>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

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
    //typename J::serializer()(v, std::inserter(*d, d->end()) );
    typename J::serializer()(v, std::back_inserter(*d) );
    t.get_aspect().template get<_output_>()(t, std::move(d));
    if (_reserve < d->size() )
      _reserve = d->size();
    if (_reserve > 8196 )
      _reserve = 8196;
      
  }

private:
  std::atomic<size_t> _reserve;
};

}}}}


