#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_request.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_request_json.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_notify.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_notify_json.hpp>
#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <wfc/jsonrpc/errors/errors.hpp>
#include <wfc/jsonrpc/errors/error_json.hpp>
#include <wfc/jsonrpc/method/aspect/tags.hpp>

#include <fas/aop/metalist.hpp>
#include <functional>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template<typename JReq, typename JResp/*, size_t ReserveSize = 80 TODO: в аспект!!!*/ >
struct call
{
  typedef fas::metalist::advice metatype;
  typedef _call_ tag;
  typedef call<JReq, JResp> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  typedef JReq  params_json;
  typedef JResp result_json;
  typedef typename params_json::target params_type;
  typedef typename result_json::target result_type;
  typedef typename std::unique_ptr<params_type> params_ptr;
  typedef typename std::unique_ptr<result_type> result_ptr;
  typedef typename std::unique_ptr<error> error_ptr;
  
  template<typename T, typename TT>
  void operator()(
    T& t, 
    TT& tt, 
    params_ptr req, 
    std::function< void (result_ptr, error_ptr)>&& callback
  ) const
  {
    if ( callback!=nullptr )
      this->request_(t, tt, std::move(req), std::move(callback) );
    else
      this->notify_(t, tt, std::move(req));
  }

private:
  
  template<typename T>
  static inline void result_handler(typename T::holder_type holder, std::function< void (result_ptr, error_ptr)> callback) 
  {
    // получатель
    if ( holder.is_response() )
    {
      result_ptr pres = nullptr;
      // TODO!!! try - catch
      pres = holder.template get_result<result_json>();
      callback( std::move(pres), nullptr);
    }
    else if ( holder.is_error() )
    {
      // TODO!!! try - catch
      error_ptr perr = holder.template get_error<error_json::type>();
      callback( nullptr, std::move(perr) );
    }
    else
    {
      callback( nullptr, nullptr);
    }
  }
  

  
  template<typename T, typename TT>
  void request_(
    T& t, 
    TT& tt, 
    params_ptr req, 
    std::function< void (result_ptr, error_ptr)>&& callback
  ) const
  {
    using namespace std::placeholders;

    std::function<void(incoming_holder holder)> handler = nullptr;
    
    if ( callback!=nullptr )
      handler = std::bind(result_handler<T>, _1, callback );
      
    t.send_request( 
      tt.name(), 
      std::move(req),
      std::bind( TT::template serialize_request<T, params_json>, _1, _2, _3),
      std::move(handler)
    );
  }
  
  template<typename T, typename TT>
  void notify_(
    T& t, 
    TT& tt, 
    params_ptr req
  ) const
  {
    using namespace std::placeholders;
    //auto serializer = std::bind(notify_serializer<T>, _1, _2);
    //auto serializer = std::bind( TT::template serialize_notify<T, params_json>, _1, _2);
    t.send_notify( 
      tt.name(), 
      std::move(req),
      std::bind( TT::template serialize_notify<T, params_json>, _1, _2)
    );

  }
};

}} // wfc


