#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_result.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_result_json.hpp>
#include <wfc/callback/callback_status.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template<typename JReq, typename JResp, size_t ReserveSize = 80 >
struct call
{
  typedef fas::metalist::advice metatype;
  typedef _invoke_ tag;
  typedef call<JReq, JResp> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  typedef JReq  request_json;
  typedef JResp response_json;
  typedef typename request_json::target  request_type;
  typedef typename response_json::target response_type;
  
  
  template<typename T, typename TT>
  void operator()(T& t, TT& tt, std::unique_ptr<request_json> req, 
    std::function< void (std::unique_ptr<response_type>, std::unique_ptr<error> err)> callback
  )
  {
    
    t.send_request( 
      tt.name(), 
                   
      [](int id) -> data_ptr 
      {
        outgoing_request<response_type> request;
        request.params = std::move(req);
        request.method = tt.name(); // ???? t - handler - имя не доступно
        request.id = id;
        auto d = std::make_unique<wfc::io::data_type>();
        d->reserve(ReserveSize); 
        typedef outgoing_request_json<request_json> send_json;
        typename send_json::serializer()(request, std::inserter( *d, d->end() ));
      },

      [](incoming_holder holder)
      {
        // получатель
        if ( holder.is_response() )
        {
          callback( nullptr, nullptr)
        }
        else if ( holder.is_error() )
        {
          callback( nullptr, nullptr)
        }
      }
    );
      
    /*
    if ( callback != nullptr )
    {
      outgoing_request<response_type> request;
      request.params = std::move(req);
      request.method = tt.name(); // ???? t - handler - имя не доступно
      request.id = 777;
      auto d = std::make_unique<wfc::io::data_type>();
      d->reserve(ReserveSize); 
      typedef outgoing_request_json<request_json> send_json;
      typename send_json::serializer()(request, std::inserter( *d, d->end() ));
    }
    */
    
  }
};

}} // wfc


