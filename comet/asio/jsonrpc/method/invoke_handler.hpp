#pragma once


#include <comet/asio/jsonrpc/objects/outgoing_response.hpp>
#include <comet/asio/jsonrpc/objects/outgoing_response_json.hpp>
#include <comet/memory.hpp>
#include <fas/typemanip.hpp>
#include <unordered_set>


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

FAS_HAS_TYPENAME(has_invoke_request,  invoke_request)
FAS_HAS_TYPENAME(has_invoke_response, invoke_response)
FAS_HAS_TYPENAME(has_invoke_error,    invoke_error)


template<typename M, bool = has_invoke_error<M>::value>
struct invoke_error
{
  typedef typename M::invoke_error type;
};

template<typename M>
struct invoke_error<M, false>
{
  typedef error_json::type type;
};

template<typename M, bool = has_invoke_request<M>::value && has_invoke_response<M>::value>
class invoke_handler
  : public callback_owner
{
  typedef std::unordered_set<int> request_ids_type;
public:

  typedef typename M::invoke_request invoke_request_json;
  typedef typename M::invoke_response invoke_response_json;
  typedef typename invoke_error<M>::type invoke_error_json;

  typedef typename invoke_request_json::target invoke_request_type;
  typedef typename invoke_response_json::target invoke_response_type;
  typedef typename invoke_error_json::target invoke_error_type;

  typedef typename invoke_request_json::serializer invoke_request_serializer;
  typedef typename invoke_response_json::serializer invoke_response_serializer;
  typedef typename invoke_error_json::serializer invoke_error_serializer;

  typedef std::unique_ptr<invoke_request_type>  invoke_request_ptr;
  typedef std::unique_ptr<invoke_response_type> invoke_response_ptr;
  typedef std::unique_ptr<invoke_error_type>    invoke_error_ptr;

  invoke_handler(M& method)
    : _method(method)
  {
    //auto x = callback_owner::callback( [](int x){return callback_status::ready;} );
  }

  template<typename T, typename Itr>
  void request(T& t, time_point ts, int id, Itr beg, Itr end)
  {
    if ( _incoming_ids.count(id) != 0 )
    {
      t.get_aspect().template get<_invalid_id_>()(t, id);
      return;
    }
    
    invoke_request_ptr req = nullptr;

    if ( beg != end) try
    {
      req = std::make_unique<invoke_request_type>();
      invoke_request_serializer()(*req, beg, end);
    }
    catch(const json::json_error& e)
    {
      t.get_aspect().template get<_invalid_params_>()(t, id);
      return;
    }
    
    _incoming_ids.insert(id);

    _method.request(t, std::move(req), id, 
                    t.owner().callback<invoke_response_ptr, invoke_error_ptr>( 
                    [this, &t, ts, id]( invoke_response_ptr res, invoke_error_ptr err )->callback_status
    {
      if ( err==nullptr )
        this->response(t, std::move(res), id);
      else
        this->error(t, std::move(err), id);
      return callback_status::ready;
    }));
  }

  // async response
  template<typename T>
  void response(T& t, invoke_response_ptr result, int id)
  {
    
    typedef typename outgoing_response_json<invoke_response_json>::type outgoing_json;
    typedef typename outgoing_json::target outgoing_type;
    
    outgoing_type resp;
    resp.id = id;
    resp.result = std::move(result);
    this->_send<outgoing_json>(t, resp, id);
    
    
    /*
    if ( _incoming_ids.count(id) == 0 )
      throw;
    
    outgoing_response<invoke_response_type> resp;
    resp.id = id;
    resp.result = std::move(result);
    typedef typename outgoing_response_json<invoke_response_json>::serializer serializer;
    data_ptr data = data_ptr(new data_type() );
    data->reserve(200);
    serializer()(resp,   std::back_inserter(*data));
    t.get_aspect().template get<_invoke_>().response(t, id, std::move(data) );
    */
    
  }

  template<typename T>
  void error(T& t, invoke_error_ptr error, int id)
  {
    typedef typename outgoing_error_json<invoke_error_json>::type outgoing_json;
    typedef typename outgoing_json::target outgoing_type;
    
    outgoing_type resp;
    resp.id = std::make_unique<int>(id);
    resp.error = std::move(error);
    this->_send<outgoing_json>(t, resp, id);

    /*
    outgoing_error<invoke_error_type> resp;
    resp.id = std::make_unique<int>(id);
    resp.error = std::move(err);
    typedef typename outgoing_error_json<invoke_error_json>::serializer serializer;
    data_ptr data = data_ptr(new data_type() );
    data->reserve(200);
    serializer()(resp,   std::back_inserter(*data));
    t.get_aspect().template get<_invoke_>().error(t, id, std::move(data) );
    */
  }
  
  template<typename J, typename T>
  void _send(T& t, const typename J::target& resp, int id)
  {
    if ( _incoming_ids.count(id) == 0 )
      throw;

    _incoming_ids.erase(id);
    typedef typename J::serializer serializer;
    data_ptr data = std::make_unique<data_type>();
    data->reserve(200);
    serializer()(resp,   std::back_inserter(*data));
    t.get_aspect().template get<_output_>()(t, std::move(data) );
  }

private:
  
  M& _method;
  request_ids_type _incoming_ids;
};

template<typename M>
class invoke_handler<M, false>
{
public:
  invoke_handler(M& ){}

  void clear() {}

  template<typename T, typename Itr>
  void request(T& t, time_point ts, int id, Itr beg, Itr end)
  {
    std::cout << "class invoke_handler<M, false>" << std::endl;
    
  }
};


}}}}


