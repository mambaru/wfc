#pragma once

#include <wfc/jsonrpc/objects/outgoing_notify.hpp>
#include <wfc/jsonrpc/objects/outgoing_notify_json.hpp>

#include <fas/typemanip.hpp>

namespace wfc{ namespace jsonrpc{


FAS_HAS_TYPENAME(has_call_notify, call_notify)

template<typename M, bool B= has_call_notify<M>::value>
class call_notify_handler
{
  M& _method;
public:

  typedef typename M::call_notify call_notify_json;
  typedef typename call_notify_json::target call_notify_type;
  typedef std::unique_ptr<call_notify_type>  call_notify_ptr;
  typedef typename outgoing_notify_json<call_notify_json>::serializer outgoing_serializer;
  
  call_notify_handler(M& method)
    : _method(method) 
    , _reserve(256)
  {
  }
  
  template< typename T >
  void notify(T& t, call_notify_ptr params, stat_function stat)
  {
    outgoing_notify<call_notify_type> ntf;
    ntf.params = std::move(params);
    ntf.method = _method.name();
    
    data_ptr data = std::make_unique<data_type>();
    data->reserve(_reserve);
    
    outgoing_serializer()(ntf,   std::back_inserter(*data));
    
    if ( data->size() > _reserve )
      _reserve = data->size();
    if (_reserve > 1024*8 )
      _reserve = 1024*8;
      
    t.get_aspect().template get<_output_>()(t, std::move(data) );
    
    if ( stat )
      stat(std::chrono::high_resolution_clock::now(), true);
  }
  
private:
  size_t _reserve;
};

template<typename M>
class call_notify_handler<M, false>
{
public:

  
  call_notify_handler(M& )
  {}
};

}}


