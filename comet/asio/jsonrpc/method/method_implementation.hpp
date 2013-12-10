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
    , method_index(-1)
  {
  };

  method_implementation(const method_implementation& m) = delete;
  method_implementation& operator=(const method_implementation& m) = delete;
  

  template<typename T, typename Itr>
  void invoke_request(T& t, time_point ts, int id, Itr beg, Itr end)
  {
    invoke_handler_base::request(t, ts, id, beg, end);
  }

  // async response
  template<typename T, typename P>
  void response(T& t, P result, int id)
  {
    invoke_handler_base::response(t, std::move(result), id);
  }

  template<typename T, typename Itr>
  void invoke_notify(T& t, Itr beg, Itr end)
  {
    return invoke_notify_handler_base::notify(t, beg, end);
  }

  // remote call
  template<typename T, typename P>
  int request ( T& t, P param)
  {
    return call_handler_base::request(t, std::move(param) );
  }

  template<typename T, typename Itr>
  void invoke_response(T& t, int id,  Itr beg, Itr end)
  {
    call_handler_base::response(t, id, beg, end);
  }

  /// remote notify id
  template<typename T, typename P>
  void notify(T& t, P params)
  {
    call_notify_handler_base::notify(t, std::move(params) );
  }

  int method_index;
};


}}}}


