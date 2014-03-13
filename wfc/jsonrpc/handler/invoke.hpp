#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_result.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_result_json.hpp>
#include <wfc/callback/callback_status.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{


template<typename Handler>
struct startup:
  fas::advice<_startup_, Handler>
{};


template<typename Handler>
struct shutdown:
  fas::advice<_shutdown_, Handler>
{};


  /*: public 
{
  typedef fas::metalist::advice metatype;
  typedef _startup_ tag;
  typedef startup advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
};*/

  
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
  
  
  template<typename T>
  void operator()(T& t, incoming_holder holder, ::wfc::io::callback handler) const
  {
    try
    {
      auto req = holder.get_params<request_json>();
      if (holder.is_notify())
      {
        Handler::operator()( t, std::move(req), nullptr);
      }
      else
      {
        auto ph = std::make_shared<incoming_holder>( std::move(holder) );
        Handler::operator()( t, std::move(req), 
          [ph, handler]( std::unique_ptr<response_type> params, std::unique_ptr<error> err )
          {
            if (err != nullptr )
            {
              typedef outgoing_error_json< error_json::type >::type json_type;
              outgoing_error<error> error_message;
              error_message.error = std::move(err);
              error_message.id = std::move( ph->raw_id() );
              
              auto d = ph->detach();
              d->clear();
              typename json_type::serializer()(error_message, std::inserter( *d, d->end() ));
              // ph->handler( std::move(d) );
              handler( std::move(d) );
            }
            else
            {
              outgoing_result<response_type> result;
              result.result = std::move(params);
              result.id = ph->raw_id();
              auto d = ph->detach();
              d->clear();
              typedef outgoing_result_json<response_json> result_json;
              typename result_json::serializer()(result, std::inserter( *d, d->end() ));
              handler( std::move(d) );
            }
          } // callback 
        );
      }
    }
    catch(const json::json_error& e)
    {
      // SEND INVALID Params и перенести выше
      //t.get_aspect().template get<_invalid_params_>()(t, id);
    }
  }
};


}} // wfc


