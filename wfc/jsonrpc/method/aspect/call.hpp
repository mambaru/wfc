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
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template<typename JReq, typename JResp, size_t ReserveSize = 80 >
struct call
{
  typedef fas::metalist::advice metatype;
  typedef _call_ tag;
  typedef call<JReq, JResp> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  typedef JReq  request_json;
  typedef JResp response_json;
  typedef typename request_json::target  request_type;
  typedef typename response_json::target response_type;
  typedef typename std::unique_ptr<request_type> request_ptr;
  typedef typename std::unique_ptr<response_type> response_ptr;
  typedef typename std::unique_ptr<error> error_ptr;
  
  template<typename T, typename TT>
  void operator()(
    T& t, 
    TT& tt, 
    request_ptr req, 
    std::function< void (response_ptr, error_ptr)> callback
  ) const
  {
    if ( callback!=nullptr )
      this->request_(t, tt, std::move(req), callback);
    else
      this->notify_(t, tt, std::move(req));
  }
private:
  
  template<typename T, typename TT>
  void request_(
    T& t, 
    TT& tt, 
    request_ptr req, 
    std::function< void (response_ptr, error_ptr)> callback
  ) const
  {
    auto serializer = [](const char* name, request_ptr req, int id) -> typename T::data_ptr 
    {
      outgoing_request<request_type> request;
      request.params = std::move(req);
      request.method = name; 
      request.id = id;
      auto d = std::make_unique< typename T::data_type>();
      d->reserve(ReserveSize); 
      typedef outgoing_request_json<request_json> send_json;
      typename send_json::serializer()(request, std::inserter( *d, d->end() ));
      return std::move(d);
    };
      
    std::function<void(incoming_holder holder)> result_handler = nullptr;
    
    if ( callback!=nullptr )
    {
      result_handler = [callback](incoming_holder holder)
      {
        // получатель
        if ( holder.is_response() )
        {
          auto pres = holder.get_result<response_json>();
          /*std::make_unique<response_type>
          response_json::serializer()( 
          // TODO!!!
          */
          callback( std::move(pres), nullptr);
        }
        else if ( holder.is_error() )
        {
          // TODO!!!
          auto perr = holder.get_error<error_json::type>();
          callback( nullptr, std::move(perr) );
        }
        else
        {
          callback( nullptr, nullptr);
        }
      };
    }
      
    t.send_request( 
      tt.name(), 
      std::move(req),
      std::move(serializer),
      std::move(result_handler)
    );
  }
  
  template<typename T, typename TT>
  void notify_(
    T& t, 
    TT& tt, 
    request_ptr req
  ) const
  {
      auto serializer = [](const char* name, request_ptr req) -> typename T::data_ptr 
      {
        outgoing_notify<request_type> notify;
        notify.params = std::move(req);
        notify.method = name; 
        auto d = std::make_unique< typename T::data_type>();
        d->reserve(ReserveSize); 
        typedef outgoing_notify_json<request_json> send_json;
        typename send_json::serializer()(notify, std::inserter( *d, d->end() ));
        return std::move(d);
      };
      
    t.send_notify( 
      tt.name(), 
      std::move(req),
      std::move(serializer)
    );

  }
};

}} // wfc


