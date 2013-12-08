#pragma once

#include <comet/asio/jsonrpc/objects/outgoing_notify.hpp>
#include <comet/asio/jsonrpc/objects/outgoing_notify_json.hpp>

#include <fas/typemanip.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{


FAS_HAS_TYPENAME(has_call_notify, call_notify)

/// //////////////////////////////////////////////////////////////////

template<typename M, bool = has_call_notify<M>::value>
class call_notify_handler
{
  M& _method;
public:

  typedef typename M::call_notify call_notify_json;
  typedef typename call_notify_json::target call_notify_type;
  typedef typename call_notify_json::serializer call_notify_serializer;
  typedef std::unique_ptr<call_notify_type>  call_notify_ptr;

  call_notify_handler(M& method)
    : _method(method) 
  {
  }

  template< typename T >
  void notify(T& t, call_notify_ptr params)
  {
    outgoing_notify<call_notify_type> resp;
    resp.params = std::move(params);
    typedef typename outgoing_notify_json<call_notify_json>::serializer serializer;
    data_ptr data = data_ptr(new data_type() );
    serializer()(resp,   std::back_inserter(*data));
    t.get_aspect().template get<_invoke_>().notify(t, std::move(data) );
  }
};

template<typename M>
class call_notify_handler<M, false>
{
public:
  call_notify_handler(M& )
  {
  }
};


}}}}


