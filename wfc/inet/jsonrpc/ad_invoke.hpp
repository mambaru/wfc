#pragma once

#include <wfc/inet/types.hpp>
#include <wfc/inet/jsonrpc/objects/incoming.hpp>
#include <wfc/inet/jsonrpc/objects/incoming_json.hpp>
#include <wfc/inet/jsonrpc/errors/tags.hpp>
#include <wfc/inet/jsonrpc/tags.hpp>
#include <fas/xtime.hpp>


namespace wfc{ namespace inet{ namespace jsonrpc{

struct f_init_index
{
  int count;
  std::function<int()> create_id;
  f_init_index(std::function<int()> create_id)
    : count(0)
    , create_id(create_id)
  {}

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    t.get_aspect().template get<Tg>().initialize(++count, create_id);
  }
};

struct f_enable_stat
{
  bool enable;
  
  f_enable_stat(bool enable)
    : enable(enable)
  {}

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    t.get_aspect().template get<Tg>().enable_stat(enable);
  }
};

struct f_invoke_request
{
  incoming& req;
  time_point ts;
  bool found;
  f_invoke_request(incoming& req, time_point ts)
    : req(req)
    , ts(ts)
    , found(false)
  {
  }

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if (found)
      return;
    
    auto& handler = t.get_aspect().template get<Tg>();
    if (std::equal(req.method.first, req.method.second, handler.name()))
    {
      handler.invoke_request(t, ts, *req.id, req.params.first, req.params.second);
      found = true;
    }
  }
};

struct f_invoke_notify
{
  incoming& req;
  time_point ts;
  bool found;
  
  f_invoke_notify(incoming& req, time_point ts)
    : req(req)
    , ts(ts)
    , found(false)
  {
  }

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if (found)
      return;
      
    auto& handler = t.get_aspect().template get<Tg>();

    if (std::equal(req.method.first, req.method.second, handler.name()))
    {
      handler.invoke_notify(t, ts, req.params.first, req.params.second);
      found = true;
    }
  }
};

struct f_invoke_response
{
  incoming& req;
  time_point ts;
  bool found;
  
  f_invoke_response(incoming& req, time_point ts)
    : req(req)
    , ts(ts)
    , found(false)
  {
  }

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if (found)
      return;
    
    found = t.get_aspect().template get<Tg>().invoke_response(t, ts, *req.id, req.result.first, req.result.second);
  }
};

struct f_invoke_error
{
  incoming& req;
  time_point ts;
  bool found;
  
  f_invoke_error(incoming& req, time_point ts)
    : req(req)
    , ts(ts)
    , found(false)
  {
  }

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if (found)
      return;
    
    found = t.get_aspect().template get<Tg>()
             .invoke_error(t, ts, *req.id, req.result.first, req.result.second);
  }
};


class ad_invoke
{
public:
  ad_invoke()
    : _id_counter(0)
  {
    _create_id = [this](){ return ++_id_counter;};
  }

  template<typename T>
  void initialize(T& t)
  {
    fas::for_each_group<_method_>(t, f_init_index(_create_id) );
  }

  template<typename T>
  void enable_stat(T& t, bool enable)
  {
    fas::for_each_group<_method_>(t, f_enable_stat(enable) );
  }

  
  int create_id() 
  {
    return ++_id_counter; 
  }

  template<typename T>
  void operator() (T& t, data_ptr data)
  {
    auto beg = data->begin();
    auto end = data->end();

    for (;beg!=end;)
    {
      time_point start = std::chrono::high_resolution_clock::now();
      if (*beg!='{')
      {
        data->erase(data->begin(), beg);
        t.get_aspect().template get<_not_jsonrpc_>()(t, std::move(data) );
        return;
      }
      else
      {
        incoming req;
        try
        {
          beg = incoming_json::serializer()(req, beg, end);
          beg = json::parser::parse_space(beg, end);
        }
        catch(const json::json_error& e)
        {
          t.get_aspect().template get<_invalid_request_>()(t);
          t.get_aspect().template get<_invalid_json_>()(t, e, beg, end);
          return;
        }
        
        _invoke(t, req, start);
      }
    }
  }
  
private:

  template<typename T>
  void _invoke(T& t, incoming& req, time_point start)
  {
    if ( req.method.first!=req.method.second )
    {
      if ( *req.method.first=='"' && *(req.method.second-1)=='"')
      {
        ++req.method.first;
        --req.method.second;
        if (req.id!=nullptr)
          _invoke_request(t, req, start);
        else
          _invoke_notify(t, req, start);
      }
      else
      {
        t.get_aspect().template get<_invalid_request_>()(t);
      }
    }
    else if ( req.result.first!=req.result.second )
    {
      _invoke_result(t, req, start);
    }
    else if ( req.error.first!=req.error.second )
    {
      _invoke_error(t, req, start);
    }
    else
    {
      t.get_aspect().template get<_invalid_request_>()(t);
    }
  }

  template<typename T>
  void _invoke_request(T& t, incoming& req, time_point start)
  {
    if ( !fas::for_each_group<_method_>(t, f_invoke_request(req, start) ).found )
    {
      t.get_aspect().template get<_method_not_found_>()(t, *req.id);
    }
  }
  
  template<typename T>
  void _invoke_notify(T& t, incoming& req, time_point start)
  {
    if ( fas::for_each_group<_method_>(t, f_invoke_notify(req, start) ).found )
    {
    }
    else
    {
      //t.get_aspect().template get<_method_not_found_>()(t);
    }
  }
  
  template<typename T>
  void _invoke_result(T& t, incoming& req, time_point start)
  {
    if ( fas::for_each_group<_method_>(t, f_invoke_response(req, start) ).found )
    {
       // TODO: notify not found
    }
  }

  template<typename T>
  void _invoke_error(T& t, incoming& req, time_point start)
  {
    if ( fas::for_each_group<_method_>(t, f_invoke_error(req, start) ).found )
    {
       // TODO: error not found
    }
  }
  
private:
  int _id_counter;
  std::function<int()> _create_id;

};

}}}


