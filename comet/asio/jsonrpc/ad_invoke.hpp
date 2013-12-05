#pragma once

#include <comet/asio/types.hpp>
#include <comet/asio/jsonrpc/request.hpp>
#include <comet/asio/jsonrpc/request_json.hpp>
#include <comet/asio/jsonrpc/tags.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct f_invoke_request
{
  int count;
  incoming& req;
  f_invoke_request(incoming& req)
    : req(req)
    , count(0)
  {
  }

  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    
    auto& handler = t.get_aspect().template get<Tg>();
    std::cout << "f_invoke_request " << handler.name() << std::endl;
    //if ( std::strncmp(handler.name(), req.method.first, std::distance(req.method.first, req.method.second) ) )
    if (std::equal(req.method.first, req.method.second, handler.name()))
    {
      handler.invoke_request(t, req.id, req.params.first, req.params.second);
      ++count;
    }
  }
};

class ad_invoke
{
  typedef std::multiset<int> ids_set;
  ids_set _in_ids;
  int _id_counter;

public:

  ad_invoke(): _id_counter(0){}
  
  size_t ids_size() const { return _in_ids.size();}

  int create_id() { return ++_id_counter; }

  template<typename T>
  void operator() (T& t, data_ptr data)
  {
    std::cout << "ad_invoke" << std::endl;
    auto beg = data->begin();
    auto end = data->end();

    for (;beg!=end;)
    {
      beg = json::parser::parse_space(beg, end);
      if (*beg!='{')
      {
        data->erase(data->begin(), beg);
        std::cout << "_not_jsonrpc_" << std::endl;
        t.get_aspect().template get<_not_jsonrpc_>()(t, std::move(data) );
        return;
      }

      incoming req;
      beg = incoming_json::serializer()(req, beg, end);
      _proccess(t, req);
    }
  }

  template<typename T>
  void _proccess(T& t, incoming& req)
  {
    if ( req.method.first!=req.method.second )
    {
      std::cout << "----" << std::string(req.method.first, req.method.second) << std::endl;
      if ( *req.method.first=='"' && *(req.method.second-1)=='"')
      {
        ++req.method.first;
        --req.method.second;

        std::cout << std::string(req.method.first, req.method.second) << std::endl;
      
        if (req.id!=-1)
          _proccess_request(t, req);
        else
          _proccess_notify(t, req);
      }
    }
    else if ( req.result.first!=req.result.second )
    {
      _proccess_result(t, req);
    }
    else if ( req.error.first!=req.error.second )
    {
      _proccess_result(t, req);
    }
    else
      _send_error(t, req);
  }

  template<typename T>
  void _proccess_request(T& t, incoming& req)
  {
    fas::for_each_group<_method_>(t, f_invoke_request(req) );
  }
  
  template<typename T>
  void _proccess_notify(T& t, incoming& req)
  {
  }
  
  template<typename T>
  void _proccess_result(T& t, incoming& req)
  {
  }

  template<typename T>
  void _proccess_error(T& t, incoming& req)
  {
  }

  template<typename T>
  void _send_error(T& t, const incoming& req)
  {
    std::cout << "_send_error" << std::endl;
  }

  
  template<typename T>
  void request(T& t, int /*id*/, const char* d, size_t s)
  {
    /*t.get_aspect().template get<_writer_>()(t, d, s);*/
  }

  template<typename T>
  void response(T& t, int id, data_ptr resp)
  {
    t.get_aspect().template get<_outgoing_>()(t, std::move(resp) );
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
  void notify(T& t, const char* d, size_t s)
  {
    /* t.get_aspect().template get<_writer_>()(t, d, s );*/
  }

  template<typename T>
  void error(T& t, int id, const char* d, size_t s)
  {
    /*
    ids_set::iterator itr = _in_ids.lower_bound(id);
    if ( itr != _in_ids.end() && (*itr) == id )
      _in_ids.erase(itr);
    t.get_aspect().template get<_writer_>()(t, d, s );
    */
  }
};

}}}}


