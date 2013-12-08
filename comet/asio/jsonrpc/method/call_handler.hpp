#pragma once


#include <comet/asio/jsonrpc/objects/outgoing_request.hpp>
#include <comet/asio/jsonrpc/objects/outgoing_request_json.hpp>

#include <fas/typemanip.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

FAS_HAS_TYPENAME(has_call_request, call_request)
FAS_HAS_TYPENAME(has_call_response, call_response)

template<typename M, bool = has_call_request<M>::value && has_call_response<M>::value>
class call_handler
{
  M& _method;

public:

  typedef typename M::call_request call_request_json;
  typedef typename M::call_response call_response_json;

  typedef typename call_request_json::target call_request_type;
  typedef typename call_response_json::target call_response_type;

  typedef typename call_request_json::serializer call_request_serializer;
  typedef typename call_response_json::serializer call_response_serializer;

  typedef std::unique_ptr<call_request_type>  call_request_ptr;
  typedef std::unique_ptr<call_response_type> call_response_ptr;
  
  call_handler(M& method)
    : _method(method)
  {}
 
  template<typename T>
  int request( T& t, call_request_ptr params)
  {
    outgoing_request<call_request_type> resp;
    resp.id = t.get_aspect().template get<_invoke_>().create_id();
    resp.params = std::move(params);
    resp.method = _method.name();
    typedef typename outgoing_request_json<call_request_json>::serializer serializer;
    data_ptr data = data_ptr(new data_type() );
    serializer()(resp,   std::back_inserter(*data));
    t.get_aspect().template get<_invoke_>().request(t, resp.id, std::move(data) );
    return resp.id;
  }

  template<typename T,  typename Itr >
  void response(T& t, int id, Itr beg, Itr end)
  {
    if (beg==end)
      return;
    
    call_response_ptr resp = std::unique_ptr<call_response_type>(new call_response_type());
    call_response_serializer()(*resp, beg, end);
    _method.response(t, std::move(resp), id);
  }
};


template<typename M>
class call_handler<M, false>
{  
public:
  call_handler(M& ){}

  template<typename T,  typename Itr >
  void response(T& t, int id, Itr beg, Itr end)
  {
    
  }
};


}}}}


