#pragma once

#include <wfc/jsonrpc/objects/outgoing_response.hpp>
#include <wfc/jsonrpc/objects/outgoing_response_json.hpp>
#include <wfc/memory.hpp>
#include <fas/typemanip.hpp>
#include <unordered_set>
#include <memory>
#include <chrono>

namespace wfc{ namespace jsonrpc{

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
  
  typedef typename outgoing_response_json<invoke_response_json>::type output_response_json;
  typedef typename output_response_json::target output_response_type;
  typedef typename outgoing_error_json<invoke_error_json>::type output_error_json;
  typedef typename output_error_json::target output_error_type;

  typedef std::unique_ptr<invoke_request_type>  invoke_request_ptr;
  typedef std::unique_ptr<invoke_response_type> invoke_response_ptr;
  typedef std::unique_ptr<invoke_error_type>    invoke_error_ptr;

  invoke_handler(M& method)
    : _method(method)
    , _reserve(256)
  {
  }

  template<typename T, typename Itr>
  void request(T& t, /*time_point ts,*/ int id, Itr beg, Itr end,  stat_function stat)
  {
    if ( _incoming_ids.count(id) != 0 )
    {
      t.get_aspect().template get<_invalid_id_>()(t, id);
      return;
    }
    
    
    invoke_request_ptr req = nullptr;

    if ( beg!=end && *beg!='n') try
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
                    t.owner().template callback<invoke_response_ptr, invoke_error_ptr>( 
                    [this, &t, stat, id]( invoke_response_ptr res, invoke_error_ptr err )->callback_status
    {
      if ( err==nullptr )
        this->response(t, std::move(res), id);
      else
        this->error(t, std::move(err), id);
      //time_point finish = std::chrono::high_resolution_clock::now();
      if (stat != nullptr)
        stat( std::chrono::high_resolution_clock::now(), err==nullptr);
      //t.get_aspect().template get<_method_stat_>()(t, this->_
      return callback_status::ready;
    }));
  }

private:

  template<typename T>
  void response(T& t, invoke_response_ptr result, int id)
  {
    output_response_type resp;
    resp.id = id;
    resp.result = std::move(result);
    this->_send<output_response_json>(t, resp, id);
  }

  template<typename T>
  void error(T& t, invoke_error_ptr error, int id)
  {
    output_error_type resp;
    resp.id = std::make_unique<int>(id);
    resp.error = std::move(error);
    this->_send<output_error_json>(t, resp, id);
  }
  
  template<typename J, typename T>
  void _send(T& t, const typename J::target& resp, int id)
  {
    if ( _incoming_ids.count(id) == 0 )
      throw invalid_id(id);

    _incoming_ids.erase(id);
    typedef typename J::serializer serializer;
    data_ptr data = std::make_unique<data_type>();
    data->reserve(_reserve);
    serializer()(resp,   std::back_inserter(*data));
    if ( data->size() > _reserve )
      _reserve = data->size();
    if ( _reserve > 1024*8 )
      _reserve = 1024*8;
    t.get_aspect().template get<_output_>()(t, std::move(data) );
  }

private:
  
  M& _method;
  request_ids_type _incoming_ids;
  size_t _reserve;
};

template<typename M>
class invoke_handler<M, false>
{
public:
  invoke_handler(M& ){}

  void clear() {}

  template<typename T, typename Itr>
  void request(T& t, int id, Itr beg, Itr end,  stat_function stat)
  {
    t.get_aspect().template get<_method_not_impl_>()(t, id);
    if (stat != nullptr)
      stat( std::chrono::high_resolution_clock::now(), true);

  }
};

}}
