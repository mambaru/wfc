#pragma once

#include <comet/asio/jsonrpc/objects/outgoing_request.hpp>
#include <comet/asio/jsonrpc/objects/outgoing_request_json.hpp>

#include <fas/typemanip.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

FAS_HAS_TYPENAME(has_call_request,  call_request)
FAS_HAS_TYPENAME(has_call_response, call_response)
FAS_HAS_TYPENAME(has_call_error,    call_error)


template<typename M, bool = has_call_error<M>::value>
struct call_error
{
  typedef typename M::call_error type;
};

template<typename M>
struct call_error<M, false>
{
  typedef error_json::type type;
};


template<typename M, bool = has_call_request<M>::value && has_call_response<M>::value>
class call_handler
{
public:

  typedef typename M::call_request     call_request_json;
  typedef typename M::call_response    call_response_json;
  typedef typename call_error<M>::type call_error_json;

  typedef typename call_request_json::target call_request_type;
  typedef typename call_response_json::target call_response_type;
  typedef typename call_error_json::target call_error_type;

  typedef typename call_request_json::serializer call_request_serializer;
  typedef typename call_response_json::serializer call_response_serializer;
  typedef typename call_error_json::serializer call_error_serializer;

  typedef std::unique_ptr<call_request_type>  call_request_ptr;
  typedef std::unique_ptr<call_response_type> call_response_ptr;
  typedef std::unique_ptr<call_error_type> call_error_ptr;
  
  typedef std::function<callback_status(call_response_ptr, call_error_ptr)> callback_type;
  
  typedef std::unordered_map<int, callback_type> callback_map;
  
  call_handler(M& method)
    : _method(method)
    , _reserve(256)
  {}
 
  template<typename T>
  int request( T& t, call_request_ptr params, callback_type callback)
  {
    outgoing_request<call_request_type> resp;
    resp.id = _create_id();
    resp.params = std::move(params);
    resp.method = _method.name();
    typedef typename outgoing_request_json<call_request_json>::serializer serializer;
    data_ptr data = std::make_unique<data_type>();
    data->reserve(_reserve);
    
    serializer()(resp,   std::back_inserter(*data));
    
    if ( data->size() > _reserve )
      _reserve = data->size();
    if (_reserve > 1024*8 )
      _reserve = 1024*8;
      
    _callback_map[resp.id] = callback;

    t.get_aspect().template get<_output_>()(t, std::move(data) );

    return resp.id;
  }

  template<typename T,  typename Itr >
  bool response(T& t, time_point ts, int id, Itr beg, Itr end)
  {
    return _call<call_response_json>(t, id, beg, end,[this](callback_type f, call_response_ptr resp){
      f( std::move(resp), nullptr);
    });
  }

  template<typename T,  typename Itr >
  bool error(T& t, time_point ts, int id, Itr beg, Itr end)
  {
    _call<call_error_json>(t, id, beg, end,[this](callback_type f, call_error_ptr err){
      f(nullptr, std::move(err) );
    });
  }

  void initialize( std::function<int()> ci)
  {
    _create_id = ci;
  }

private:

  template<typename J, typename T, typename Itr, typename F>
  bool _call(T& t, int id, Itr beg, Itr end, F f)
  {
    auto itr = _callback_map.find(id);
    if ( itr == _callback_map.end() )
      return false;

    typedef J json_type;
    typedef typename J::target target_type;
    typedef std::unique_ptr<target_type> target_ptr;

    target_ptr resp;
    if (beg!=end && *beg!='n')
    {
      resp = std::make_unique<target_type>();
      try
      {
        typename json_type::serializer()(*resp, beg, end);
      }
      catch(const json::json_error& e)
      {
        t.get_aspect().template get<_invalid_json_>()(t, e, beg, end);
      }
    }
    
    f( itr->second, std::move(resp) );
    _callback_map.erase(itr);
    return true;
  }

private:
  M& _method;
  size_t _reserve;
  callback_map _callback_map;
  std::function<int()> _create_id;
};


template<typename M>
class call_handler<M, false>
{  
public:
  call_handler(M&)
  {
    
  }

  template<typename T,  typename Itr >
  void response(T& t, int id, Itr beg, Itr end)
  {
    
  }

  template<typename T,  typename Itr >
  void error(T& t, int id, Itr beg, Itr end)
  {
    
  }
  
  void initialize( std::function<int()> ci)
  {
  }
};


}}}}


