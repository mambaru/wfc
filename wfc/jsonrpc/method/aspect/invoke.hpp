#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_result.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_result_json.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  

template<typename JReq, typename JResp, typename Handler>
struct invoke: Handler
{
  typedef fas::metalist::advice metatype;
  typedef _invoke_ tag;
  typedef invoke<JReq, JResp, Handler> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  typedef JReq  request_json;
  typedef JResp response_json;
  typedef typename request_json::target  request_type;
  typedef typename response_json::target response_type;
  
  template<typename T, typename TT>
  void operator()(
    T& t, 
    TT&,
    typename T::holder_type holder, 
    typename T::outgoing_handler_t outgoing_handler
  ) 
  {
    try
    {
      std::unique_ptr<typename request_json::target> req = nullptr;

      try
      {
        req = holder.template get_params<request_json>();
      }
      catch (const json::json_error& )
      {
        TT::template invoke_error<T, error_json>( std::move(holder), std::make_unique<invalid_params>(), std::move(outgoing_handler) );
        return;
      }
      
      if (holder.is_notify())
      {
        Handler::operator()( t, std::move(req), nullptr);
      }
      else
      {
        auto ph = std::make_shared<incoming_holder>( std::move(holder) );
        Handler::operator()( t, std::move(req), 
          [ph, outgoing_handler]( std::unique_ptr<response_type> params, std::unique_ptr<error> err )
          {
            if (err == nullptr )
            {
              TT::template invoke_result<T, response_json>( std::move(*ph), std::move(params), std::move(outgoing_handler) );
              /*
              outgoing_result<response_type> result_message;
              result_message.result = std::move(params);
              
              auto id_range = ph->raw_id();
              result_message.id = std::make_unique<typename T::data_type>( id_range.first, id_range.second );

              auto d = ph->detach();
              d->clear();
              typedef outgoing_result_json<response_json> result_json;
              typename result_json::serializer()(result_message, std::inserter( *d, d->end() ));
              outgoing_handler( std::move(d) );
              */
            }
            else
            {
              TT::template invoke_error<T, error_json>( std::move(*ph), std::move(err), std::move(outgoing_handler) );
            }
          } // callback 
        );
      }
    }
    catch(...)
    {
      //TT::template invoke_error<T. error_json>( std::move(holder), std::make_unique<server_error>  (), std::move(outgoing_handler) );
      //TT::template invoke_error<T, error_json>( std::move(holder), std::make_unique<invalid_params>(), std::move(outgoing_handler) );
      TT::template invoke_error<T, error_json>( std::move(holder), std::make_unique<server_error>(), std::move(outgoing_handler) );
    }
  }
  
private:
};


}} // wfc


