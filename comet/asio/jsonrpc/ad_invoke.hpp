#pragma once

#include <comet/asio/types.hpp>
#include <comet/asio/jsonrpc/request.hpp>
#include <comet/asio/jsonrpc/request_json.hpp>
#include <comet/asio/jsonrpc/tags.hpp>
#include <fas/xtime.hpp>


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct f_init_index
{
  int count;
  f_init_index()
    : count(0)
  {}

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    t.get_aspect().template get<Tg>().method_index = ++count;
  }

};
  
struct f_invoke_request
{
  bool found;
  incoming& req;
  f_invoke_request(incoming& req)
    : req(req)
    , found(false)
  {
  }

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if (found)
      return;
      
    auto& handler = t.get_aspect().template get<Tg>();
    // std::cout << "f_invoke_request " << handler.name() << std::endl;
    //if ( std::strncmp(handler.name(), req.method.first, std::distance(req.method.first, req.method.second) ) )
    if (std::equal(req.method.first, req.method.second, handler.name()))
    {
      handler.invoke_request(t, req.id, req.params.first, req.params.second);
      found = true;
    }
  }
};

struct f_invoke_notify
{
  bool found;
  incoming& req;
  f_invoke_notify(incoming& req)
    : req(req)
    , found(false)
  {
  }

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if (found)
      return;
      
    auto& handler = t.get_aspect().template get<Tg>();
    // std::cout << "f_invoke_notify " << handler.name() << std::endl;
    //if ( std::strncmp(handler.name(), req.method.first, std::distance(req.method.first, req.method.second) ) )
    if (std::equal(req.method.first, req.method.second, handler.name()))
    {
      fas::nanospan ns = fas::nanotime();
      handler.invoke_notify(t, req.params.first, req.params.second);
      fas::nanospan nf = fas::nanotime();


      std::cout << "method timeout " << nf-ns << std::endl;
      std::cout << "method rate " << fas::rate(nf-ns) << std::endl;

      
      found = 0;
    }
  }
};

struct f_invoke_response
{
  bool found;
  incoming& req;
  f_invoke_response(incoming& req)
    : req(req)
    , found(false)
  {
  }

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if (found)
      return;
      
    auto& handler = t.get_aspect().template get<Tg>();
    std::cout << "f_invoke_response " << handler.name() << std::endl;
    //if ( std::strncmp(handler.name(), req.method.first, std::distance(req.method.first, req.method.second) ) )
    if (handler.has_id(req.id) )
    {
      handler.invoke_response(t, req.id, req.result.first, req.result.second);
      found = 0;
    }
  }
};


class ad_invoke
{
  typedef std::chrono::high_resolution_clock::time_point time_point;
  typedef std::pair<time_point, int> request_stat;
  typedef std::unordered_map<int, request_stat> request_ids_type;

  request_ids_type _incoming_ids;
  request_ids_type _outgoing_ids;
  //typedef std::multiset<int> ids_set;
  //ids_set _in_ids;
  int _id_counter;

public:

  ad_invoke()
    : _id_counter(0)
  {
    
  }

  template<typename T>
  void initialize(T& t)
  {
    std::cout << "initialize" << std::endl;
    fas::for_each_group<_method_>(t, f_init_index() );
  }

  const request_ids_type& incoming_ids() const { return _incoming_ids;}
  const request_ids_type& outgoing_ids() const { return _outgoing_ids;}
  //size_t ids_size() const { return _in_ids.size();}

  int create_id() { return ++_id_counter; }

  template<typename T>
  void operator() (T& t, data_ptr data)
  {
    auto beg = data->begin();
    auto end = data->end();

    for (;beg!=end;)
    {
      
      time_point start = std::chrono::high_resolution_clock::now();
      beg = json::parser::parse_space(beg, end);
      if (*beg!='{')
      {
        data->erase(data->begin(), beg);
        t.get_aspect().template get<_not_jsonrpc_>()(t, std::move(data) );
        return;
      }

      incoming req;
      
      
      beg = incoming_json::serializer()(req, beg, end);
      
      fas::nanospan ns = fas::nanotime();
      _invoke(t, req, start);
      fas::nanospan nf = fas::nanotime();
      

      std::cout << "timeout " << nf-ns << std::endl;
      std::cout << "rate " << fas::rate(nf-ns) << std::endl;

    }
  }

  template<typename T>
  void _invoke(T& t, incoming& req, time_point start)
  {
    if ( req.method.first!=req.method.second )
    {
      if ( *req.method.first=='"' && *(req.method.second-1)=='"')
      {
        ++req.method.first;
        --req.method.second;

        std::cout << std::string(req.method.first, req.method.second) << std::endl;
      
        if (req.id>0)
          _invoke_request(t, req, start);
        else
          _invoke_notify(t, req, start);
      }
    }
    else if ( req.result.first!=req.result.second )
    {
      _invoke_result(t, req, start);
    }
    else if ( req.error.first!=req.error.second )
    {
      _invoke_result(t, req, start);
    }
    else
      _send_error(t, req, start);
  }

  template<typename T>
  void _invoke_request(T& t, incoming& req, time_point start)
  {
    auto itr = _incoming_ids.find(req.id);
    _incoming_ids[req.id]=std::make_pair(start, -1);
    if ( itr==_incoming_ids.end())
    {
      if ( !fas::for_each_group<_method_>(t, f_invoke_request(req) ).found )
      {
        _incoming_ids.erase(req.id);
        ; // method not found
      }
    }
    else
    {
      // invalid id
    }
  }
  
  template<typename T>
  void _invoke_notify(T& t, incoming& req, time_point start)
  {
    if ( fas::for_each_group<_method_>(t, f_invoke_notify(req) ).found )
    {
      time_point finish = std::chrono::high_resolution_clock::now();
      long int ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
    }
    else
    {}
  }
  
  template<typename T>
  void _invoke_result(T& t, incoming& req, time_point start)
  {
    auto itr = _outgoing_ids.find(req.id);
    if ( itr == _outgoing_ids.end() )
    {
      if ( fas::for_each_group<_method_>(t, f_invoke_response(req) ).found )
        ; // notify not found
    }
  }

  template<typename T>
  void _invoke_error(T& t, incoming& req, time_point start)
  {
    auto itr = _incoming_ids.find(req.id);
    _incoming_ids.erase(itr);
    // TDDO
  }

  template<typename T>
  void _send_error(T& t, const incoming& req, time_point start)
  {
    std::cout << "_send_error" << std::endl;
  }

  
  template<typename T>
  void request(T& t, int id, data_ptr req)
  {
    auto itr = _outgoing_ids.find(id);
    if ( itr == _outgoing_ids.end() )
    {
      // TODO: method index
      _outgoing_ids[id]=std::make_pair(std::chrono::high_resolution_clock::now(), -1);
      t.get_aspect().template get<_outgoing_>()(t, std::move(req) );
    }
    
  }

  template<typename T>
  void response(T& t, int id, data_ptr resp)
  {
    
    auto itr = _incoming_ids.find(id);
    if ( itr != _incoming_ids.end() )
    {
      t.get_aspect().template get<_outgoing_>()(t, std::move(resp) );

      
      time_point finish = std::chrono::high_resolution_clock::now();
      //method_index = -1;
      long int ts = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - itr->second.first).count();
      // send stat
      _incoming_ids.erase(itr);
      //std::cout << "timeout " << ts << std::endl;
      //std::cout << "rate " << double(1000000000)/ts << std::endl;

      
    }
    else
    {
      // invalid id
    }


    /*
    ::mamba::comet::inet::jsonrpc::response resp;
    resp.id = id;
    resp.result = std::move(result);
    typedef ::mamba::comet::inet::jsonrpc::response_json::serializer serializer;
    data_ptr json = data_ptr(new data_type());
    serializer()(resp, std::back_inserter(*json));
    */
    
    
    // std::cout << std::string(result->begin(), result->end());
    /*
    ids_set::iterator itr = _in_ids.lower_bound(id);
    if ( itr != _in_ids.end() && (*itr) == id )
      _in_ids.erase(itr);
    t.get_aspect().template get<_writer_>()(t, d, s );
    */
  }

  template<typename T>
  void notify(T& t, data_ptr resp)
  {
    t.get_aspect().template get<_outgoing_>()(t, std::move(resp) );
    /* t.get_aspect().template get<_writer_>()(t, d, s );*/
  }

  template<typename T>
  void error(T& t, int id, data_ptr resp)
  {
    auto itr = _incoming_ids.find(id);
    if ( itr != _incoming_ids.end() )
    {
      t.get_aspect().template get<_outgoing_>()(t, std::move(resp) );
      time_point finish = std::chrono::high_resolution_clock::now();
      long int ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - itr->second.first).count();
      // send stat
      _incoming_ids.erase(id);
    }

    /*
    ids_set::iterator itr = _in_ids.lower_bound(id);
    if ( itr != _in_ids.end() && (*itr) == id )
      _in_ids.erase(itr);
    t.get_aspect().template get<_writer_>()(t, d, s );
    */
  }
};

}}}}


