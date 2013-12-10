#pragma once

#include <comet/asio/jsonrpc/method/invoke_handler.hpp>
#include <comet/asio/jsonrpc/method/call_handler.hpp>

#include <fas/typemanip.hpp>


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{


FAS_HAS_TYPENAME(has_invoke_notify, invoke_notify)


/// //////////////////////////////////////////////////////////////////


template<typename M, bool = has_invoke_notify<M>::value >
class invoke_notify_handler
{
  M& _method;
public:

  typedef typename M::invoke_notify invoke_notify_json;
  typedef typename invoke_notify_json::target invoke_notify_type;
  typedef typename invoke_notify_json::serializer invoke_notify_serializer;
  typedef std::unique_ptr<invoke_notify_type>  invoke_notify_ptr;

  invoke_notify_handler(M& method)
    : _method(method) 
  {
  }

  template<typename T,  typename Itr>
  void notify(T& t, Itr beg, Itr end)
  {
    if ( beg == end) return;

    invoke_notify_ptr req = invoke_notify_ptr(new invoke_notify_type() );

    try
    {
      invoke_notify_serializer()(*req, beg, end);
    }
    catch(...)
    {
      // NOTIFY не отправлять!!!
      //t.get_aspect().template get<_invalid_json_>()(t, beg, end);
      return;
    }

    _method.notify(t, std::move(req) );

  }
};

template<typename M>
class invoke_notify_handler<M, false>
{
public:
  invoke_notify_handler(M& ){}
  template<typename T,  typename Itr>
  void notify(T& t, Itr beg, Itr end)
  {
  }
};


/// //////////////////////////////////////////////////////////////////



}}}}


