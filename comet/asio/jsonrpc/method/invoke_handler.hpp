#pragma once


#include <comet/asio/jsonrpc/objects/outgoing_response.hpp>
#include <comet/asio/jsonrpc/objects/outgoing_response_json.hpp>
#include <fas/typemanip.hpp>


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

FAS_HAS_TYPENAME(has_invoke_request,  invoke_request)
FAS_HAS_TYPENAME(has_invoke_response, invoke_response)
//FAS_HAS_TYPENAME(has_invoke_error, invoke_response)

template<typename M, bool = has_invoke_request<M>::value && has_invoke_response<M>::value>
class invoke_handler
{
  M& _method;
public:

  typedef typename M::invoke_request invoke_request_json;
  typedef typename M::invoke_response invoke_response_json;

  typedef typename invoke_request_json::target invoke_request_type;
  typedef typename invoke_response_json::target invoke_response_type;

  typedef typename invoke_request_json::serializer invoke_request_serializer;
  typedef typename invoke_response_json::serializer invoke_response_serializer;

  typedef std::unique_ptr<invoke_request_type>  invoke_request_ptr;
  typedef std::unique_ptr<invoke_response_type> invoke_response_ptr;

  invoke_handler(M& method)
    : _method(method)
  {
    //TODO: last reserve
  }

  template<typename T, typename Itr>
  void request(T& t, int id, Itr beg, Itr end)
  {
    if ( beg == end) return;

    invoke_request_ptr req = invoke_request_ptr(new invoke_request_type() );

    try
    {
      invoke_request_serializer()(*req, beg, end);
    }
    catch(...)
    {
      t.get_aspect().template get<_invalid_json_>()(t, beg, end);
      return;
    }

    
    _method.request(t, std::move(req), id, [this, &t, id](invoke_response_ptr res/*, error!!!**/)
    {
      this->response(t, std::move(res), id);
    });
  }

  // async response
  template<typename T>
  void response(T& t, invoke_response_ptr res, int id)
  {
    outgoing_response<invoke_response_type> resp;
    resp.id = id;
    resp.result = std::move(res);
    typedef typename outgoing_response_json<invoke_response_json>::serializer serializer;
    data_ptr data = data_ptr(new data_type() );
    data->reserve(200);
    serializer()(resp,   std::back_inserter(*data));
    t.get_aspect().template get<_invoke_>().response(t, id, std::move(data) );
  }
};

template<typename M>
class invoke_handler<M, false>
{
public:
  invoke_handler(M& ){}

  void clear() {}

  template<typename T, typename Itr>
  void request(T& t, int id, Itr beg, Itr end)
  {
    std::cout << "class invoke_handler<M, false>" << std::endl;
    
  }
};


}}}}


