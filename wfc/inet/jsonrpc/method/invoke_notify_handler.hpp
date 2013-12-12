#pragma once

#include <wfc/inet/jsonrpc/method/invoke_handler.hpp>
#include <wfc/inet/jsonrpc/method/call_handler.hpp>

#include <fas/typemanip.hpp>


namespace wfc{ namespace inet{ namespace jsonrpc{


FAS_HAS_TYPENAME(has_invoke_notify, invoke_notify)

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
  void notify(T& t, Itr beg, Itr end, stat_function stat)
  {
    invoke_notify_ptr req = nullptr;

    if ( beg!=end && *beg!='n') try
    {
      req = std::make_unique<invoke_notify_type>();
      invoke_notify_serializer()(*req, beg, end);
    }
    catch(const json::json_error& e)
    {
      t.get_aspect().template get<_invalid_json_>()(t, e, beg, end);
      return;
    }

    _method.notify(t, std::move(req) );
    
    if (stat != nullptr)
      stat( std::chrono::high_resolution_clock::now(), true);
  }
};

template<typename M>
class invoke_notify_handler<M, false>
{
public:
  invoke_notify_handler(M& ){}
  template<typename T,  typename Itr>
  void notify(T& t, Itr beg, Itr end, stat_function stat)
  {
  }
};


/// //////////////////////////////////////////////////////////////////



}}}


