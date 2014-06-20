#pragma once
#include <wfc/io/types.hpp>
#include <memory>
namespace wfc{ namespace jsonrpc{
  
struct ad_callback_json
{
  template<typename T, typename J, typename O>
  void operator()( T&, J, O obj, typename T::data_ptr d, ::wfc::io::outgoing_handler_t callback)
  {
    d->clear();
    d->reserve(80);
    typename J::serializer()(obj, std::inserter( *d, d->end() ));
    callback( std::move(d) );
  }
  
  template<typename T, typename J, typename O>
  void operator()( T&, J, O obj, ::wfc::io::outgoing_handler_t handler)
  {
    auto d = std::make_unique< ::wfc::io::data_type>();
    d->reserve(80);
    typename J::serializer()(obj, std::inserter( *d, d->end() ));
    handler( std::move(d) );
  }
};
  
}} // wfc
