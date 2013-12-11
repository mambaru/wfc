#pragma once

#include <comet/asio/jsonrpc/method/invoke_handler.hpp>
#include <comet/asio/jsonrpc/method/call_handler.hpp>
#include <comet/asio/jsonrpc/method/invoke_notify_handler.hpp>
#include <comet/asio/jsonrpc/method/call_notify_handler.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

/// /////////////////////////////////////
template<typename M>
class method_implementation
  : public M
  , private invoke_handler< M >
  , private call_handler< M >
  , private invoke_notify_handler< M >
  , private call_notify_handler< M >
{
public:
  typedef method_implementation<M> self;
  typedef M method_class;
  typedef invoke_handler< M > invoke_handler_base;
  typedef call_handler< M > call_handler_base;
  typedef invoke_notify_handler< M > invoke_notify_handler_base;
  typedef call_notify_handler< M > call_notify_handler_base;

  method_implementation()
    : M()
    , invoke_handler_base( static_cast<M&>(*this) )
    , call_handler_base( static_cast<M&>(*this) )
    , invoke_notify_handler_base( static_cast<M&>(*this) )
    , call_notify_handler_base( static_cast<M&>(*this) )
    , _method_index(-1)
#warning
    , _enable_stat(true)
  {
  };

  method_implementation(const method_implementation& m) = delete;
  method_implementation& operator=(const method_implementation& m) = delete;
  

  template<typename T, typename Itr>
  void invoke_request(T& t, time_point ts, int id, Itr beg, Itr end)
  {
    std::cout <<  "invoke_request" <<  std::endl;
    stat_function stat = nullptr;
    if (_enable_stat )
    {
      std::cout <<  "invoke_request _enable_stat" <<  std::endl;
      stat = [&t, ts](time_point finish, bool succsess)
      {
        t.get_aspect().template get<_method_stat_>()(t, stat_category::invoke_request, ts, finish, succsess);
      };
    }
    invoke_handler_base::request(t,  id, beg, end,  stat);
  }

  /*
  template<typename T, typename P>
  void response(T& t, P result, int id)
  {
    invoke_handler_base::response(t, std::move(result), id);
  }
  */

  template<typename T, typename Itr>
  void invoke_notify(T& t, time_point ts, Itr beg, Itr end)
  {
    return invoke_notify_handler_base::notify(t, ts, beg, end);
  }
  
  template<typename T, typename P>
  int request ( T& t, P param, typename call_handler_base::callback_type callback)
  {
    // typedef std::function<callback_status(call_response_ptr, call_error_ptr)> callback_type;

    typename call_handler_base::callback_type callback_stat = callback;
    if (_enable_stat)
    {
      time_point start = std::chrono::high_resolution_clock::now();
      callback_stat = [&t, start, callback](typename call_handler_base::call_response_ptr resp, typename call_handler_base::call_error_ptr err)
      {
        callback_status status = callback( std::move(resp), std::move(err) );
        time_point finish = std::chrono::high_resolution_clock::now();
        t.get_aspect().template get<_method_stat_>()(t, stat_category::call_request, start, finish, status==callback_status::ready);
        return status;
      };
    }

    return call_handler_base::request(t, std::move(param), callback );
  }

  template<typename T, typename Itr>
  bool invoke_response(T& t, time_point ts, int id,  Itr beg, Itr end)
  {
    return call_handler_base::response(t, ts, id, beg, end);
  }

  template<typename T, typename Itr>
  bool invoke_error(T& t, time_point ts, int id,  Itr beg, Itr end)
  {
    return call_handler_base::error(t, ts, id, beg, end);
  }

  
  template<typename T, typename P>
  void notify(T& t, P params)
  {
    call_notify_handler_base::notify(t, std::move(params) );
  }

  int method_index() const 
  {
    return _method_index;
  } 
  
  void initialize(int method_index, std::function<int()> create_id)
  {
    _method_index = method_index;
    call_handler_base::initialize(create_id);
  }
  
private:
  int  _method_index;
  bool _enable_stat;
};


}}}}


