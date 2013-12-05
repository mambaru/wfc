#pragma once

#include <set>
#include <fas/typemanip.hpp>

#include <comet/asio/types.hpp>
#include <comet/asio/jsonrpc/response.hpp>
#include <comet/asio/jsonrpc/response_json.hpp>


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

FAS_HAS_TYPENAME(has_invoke_request, invoke_request)
// HAS_TYPE(has_invoke_response, invoke_request)
FAS_HAS_TYPENAME(has_invoke_response, invoke_response)

FAS_HAS_TYPENAME(has_call_request, call_request)
//HAS_TYPE(has_call_response, call_request)
FAS_HAS_TYPENAME(has_call_response, call_response)

FAS_HAS_TYPENAME(has_invoke_notify, invoke_notify)
FAS_HAS_TYPENAME(has_call_notify, call_notify)


template<typename M, bool = has_invoke_request<M>::value && has_invoke_response<M>::value>
class invoker
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

  invoker(M& method): _method(method) {}

  void clear() {  }

  template<typename T, typename Itr>
  void request(T& t, int id, Itr beg, Itr end)
  {
    if ( beg == end) return;

    std::cout << "class invoker<M, true>" << std::endl;
    /*
    invoke_request_rpc_type req;
    invoke_response_rpc_type res;
    */
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

    _method.request(t, std::move(req), id, [this,&t, id](invoke_response_ptr res)
    {
      this->response(t, std::move(res), id);
      /*
      data_type_ptr data = data_type_ptr(new data_type);
      invoke_response_serializer()(res, std::back_inserter(*data));
      t.get_aspect().template get<_invoke_>().response(t, id, data );
      */
    });
    
    /*
    if ( _method.request(t, req.params, req.id, res.result) )
    {
      res.id = req.id;
      std::vector<char>& v = t.get_aspect().template get<_outgoing_data_>()();
      v.clear();
      invoke_response_serializer()(res, std::back_inserter(v));
      if (!v.empty())
        t.get_aspect().template get<_invoke_>().response(t, res.id, &(v[0]), v.size() );
    }*/
    
  }

  // async response
  template<typename T>
  void response(T& t, invoke_response_ptr res, int id)
  {
    ::mamba::comet::inet::jsonrpc::response<invoke_response_type> resp;
    resp.id = id;
    resp.result = std::move(res);
    typedef typename ::mamba::comet::inet::jsonrpc::response_json<invoke_response_json>::serializer serializer;
    data_ptr data = data_ptr(new data_type() );
    serializer()(resp,   std::back_inserter(*data));
    t.get_aspect().template get<_invoke_>().response(t, id, std::move(data) );
    //t.get_aspect().template get<_invoke_>().response(t, res.id, &(v[0]), v.size() );
    /*
    ::mamba::comet::response<>
      data_ptr data = data_ptr(new data_type);
      invoke_response_serializer()(*res, std::back_inserter(*data));
      t.get_aspect().template get<_invoke_>().response(t, id, std::move(data) );
      */
    /*
    invoke_response_rpc_type res;
    res.result = result;
    res.id = id;

    std::vector<char>& v = t.get_aspect().template get<_outgoing_data_>()();
    v.clear();
    invoke_response_serializer()(res, std::back_inserter(v));
    if ( !v.empty() )
      t.get_aspect().template get<_invoke_>().response(t, res.id, &(v[0]), v.size() );
    */
  }
};


template<typename M>
class invoker<M, false>
{
public:
  invoker(M& ){}

  void clear() {}

  template<typename T, typename Itr>
  void request(T& t, int id, Itr beg, Itr end)
  {
    std::cout << "class invoker<M, false>" << std::endl;
    
  }
};


/*!
template<typename M, bool = has_call_request<M>::result && has_call_response<M>::result>
class caller
{
  M& _method;
  typedef std::set<int> ids_map;


  ids_map _out_ids;

public:

  typedef typename M::call_request call_request;
  typedef typename M::call_response call_response;

  typedef typename call_request::target call_request_type;
  typedef typename call_response::target call_response_type;

  typedef typename request_maker<call_request>::type call_request_rpc;
  typedef typename response_maker<call_response>::type call_response_rpc;

  typedef typename call_request_rpc::target call_request_rpc_type;
  typedef typename call_response_rpc::target call_response_rpc_type;

  typedef typename call_request_rpc::serializer call_request_serializer;
  typedef typename call_response_rpc::serializer call_response_serializer;

  caller(M& method): _method(method){}

  bool has_id(int id) const
  {
    return _out_ids.find(id)!=_out_ids.end();
  }
  
  size_t ids_size() const { return _out_ids.size();} 

  void get_ids(std::vector<int>& ids)
  {
    ids.clear();
    ids.reserve( _out_ids.size() );
    std::copy(_out_ids.begin(), _out_ids.end(), std::back_inserter(ids) );
  }


  void clear() { _out_ids.clear(); }

  template<typename T>
  int request ( T& t, const call_request_type& param)
  {
    call_request_rpc_type req(_method.name());
    req.params = param;
    req.id = t.get_aspect().template get<_invoke_>().create_id();
    _out_ids.insert(req.id);
    std::vector<char>& v = t.get_aspect().template get<_outgoing_data_>()();
    v.clear();
    call_request_serializer()(req, std::back_inserter(v));

    if (!v.empty())
      t.get_aspect().template get<_invoke_>().request(t, req.id, &(v[0]), v.size() );
    return req.id;
  }

  template<typename T>
  const char* response(T& t, const char* beg, size_t s)
  {
    if ( s == 0 || beg == 0) return 0;
    const char* next = 0;
    call_response_rpc_type res;

    try
    {
      next = call_response_serializer()(res, beg, beg + s);
    }
    catch(...)
    {
      next = 0;
      t.get_aspect().template get<_invalid_json_>()(t, beg, s);
      return next;
    }


    ids_map::iterator itr = _out_ids.find(res.id);
    if (  itr == _out_ids.end() )
      t.get_aspect().template get<_invalid_id_>()(t, beg, s );
    else
    {
      _method.response(t, res.result, res.id);
      _out_ids.erase(itr);
    }
    return next;
  }
};
*/


/*
template<typename M>
class caller<M, false>
{
public:
  caller(M& ){}

  bool has_id(int id) const { return false; }

  void clear() {}

  template<typename T>
  const char* response(T& , const char* beg, size_t )
  {
    return beg;
  }
  
  size_t ids_size() const { return 0;} 
};
*/

/// //////////////////////////////////////////////////////////////////

/*
template<typename M, bool = has_invoke_notify<M>::result>
class notified
{
  M& _method;
public:

  typedef typename M::invoke_notify invoke_notify;
  typedef typename invoke_notify::target invoke_notify_type;
  typedef typename notify_maker<invoke_notify>::type invoke_notify_rpc;
  typedef typename invoke_notify_rpc::target invoke_notify_rpc_type;
  typedef typename invoke_notify_rpc::serializer invoke_notify_serializer;
  // typedef typename amj::serializer<invoke_notify_rpc> invoke_notify_serializer;

  notified(M& method): _method(method) {}

  template<typename T>
  const char* notify(T& t, const char* beg, size_t s)
  {
    if ( s == 0 || beg == 0) return 0;
    const char* next = 0;
    invoke_notify_rpc_type ntf;
    try
    {
      next = invoke_notify_serializer()( ntf, beg, beg + s );
    }
    catch(...)
    {
      next = 0;
      t.get_aspect().template get<_invalid_json_>()(t, beg, s);
      return next;
    }
    _method.notify(t, ntf.params);
    return next;
  }
};

template<typename M>
class notified<M, false>
{
public:
  notified(M& ){}
  template<typename T>
  const char* notify(T& , const char* beg, size_t ) { return beg;}
};
*/
/// //////////////////////////////////////////////////////////////////

/*
template<typename M, bool = has_call_notify<M>::result>
class notifier
{
  M& _method;
public:

  typedef typename M::call_notify call_notify;
  typedef typename call_notify::target call_notify_type;
  typedef typename notify_maker<call_notify>::type call_notify_rpc;
  typedef typename call_notify_rpc::target call_notify_rpc_type;
  typedef typename call_notify_rpc::serializer call_notify_serializer;
  // typedef typename amj::serializer<call_notify_rpc> call_notify_serializer;

  notifier(M& method): _method(method) {  }

  template< typename T >
  void notify(T& t, const call_notify_type& param)
  {
    call_notify_rpc_type req(_method.name());
    req.params = param;
    std::vector<char>& v = t.get_aspect().template get<_outgoing_data_>()();
    v.clear();
    call_notify_serializer()(req, std::back_inserter(v));
    if ( !v.empty() )
       t.get_aspect().template get<_invoke_>().notify(t, &(v[0]), v.size() );
  }
};

template<typename M>
class notifier<M, false>
{
public:
  notifier(M& ){}
};
*/


/// /////////////////////////////////////
template<typename M>
class method
  : public M
  , private invoker< M >
  //! , private caller< M >
  //! , private notified< M >
  //! , private notifier< M >
{
public:
  typedef M method_class;
  typedef invoker< M > invoker_base;
  //typedef caller< M > caller_base;
  //typedef notified< M > notified_base;
  //typedef notifier< M > notifier_base;

  method()
    : M()
    , invoker_base( static_cast<M&>(*this) )
    //! , caller_base( static_cast<M&>(*this) )
    //! , notified_base( static_cast<M&>(*this) )
    //! , notifier_base( static_cast<M&>(*this) )
  {
  };

  method(const method& m) = delete;
  /*
    : M( m )
    , invoker_base( static_cast<M&>(*this) )
    , caller_base( static_cast<M&>(*this) )
    , notified_base( static_cast<M&>(*this) )
    , notifier_base( static_cast<M&>(*this) )
  {
  };*/


  /*
  template<typename T>
  void clear(T& t)
  {
    M::clear(t);
    invoker_base::clear();
    caller_base::clear();
  }*/

  size_t ids_size() const { 
    // return caller_base::ids_size();
  }
  
  void get_ids(std::vector<int>& ids)
  {
    //caller_base::get_ids(ids);
    /*
    ids.clear();
    ids.reserve( caller_base::_out_ids.size() );
    std::copy(caller_base::_out_ids.begin(), caller_base::_out_ids.end(), std::back_inserter(ids) );
    */
  }


  bool has_id(int id) const
  {
    //return caller_base::has_id(id);
  }


  template<typename T, typename Itr>
  void invoke_request(T& t, int id, Itr beg, Itr end)
  {
    invoker_base::request(t, id, beg, end);
  }

  // async response
  template<typename T, typename P>
  void response(T& t, const P& result, int id)
  {
    invoker_base::response(t, result, id);
  }

  template<typename T>
  const char* invoke_notify(T& t, const char* beg, size_t s)
  {
    //return notified_base::notify(t, beg, s);
  }

  // remote call
  template<typename T, typename P>
  int request ( T& t, const P& param)
  {
    //return caller_base::request(t, param);
  }

  template<typename T>
  const char* invoke_response(T& t, const char* beg, size_t s)
  {
    //return caller_base::response(t, beg, s);
  }

  /// remote notify
  template<typename T, typename P>
  void notify(T& t, const P& params)
  {
    //notifier_base::notify(t, params);
  }

private:
};


}}}}


