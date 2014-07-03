#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  

template<typename JParams, typename JResult, typename Handler, typename JError = error_json>
struct invoke: Handler
{
  typedef fas::metalist::advice metatype;
  typedef _invoke_ tag;
  typedef invoke<JParams, JResult, Handler> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  typedef JParams params_json;
  typedef JResult result_json;
  typedef JError  error_json;
  
  typedef typename params_json::target  params_type;
  typedef typename result_json::target  result_type;
  typedef typename error_json::target   error_type;

  typedef typename std::unique_ptr<params_type> params_ptr;
  typedef typename std::unique_ptr<result_type> result_ptr;
  typedef typename std::unique_ptr<error_type>  error_ptr;

  template<typename T, typename TT>
  void operator()(
    T& t, 
    TT&,
    typename T::holder_type holder, 
    typename T::outgoing_handler_t outgoing_handler
  ) 
  {
    params_ptr req = nullptr;

    try // invalid_params
    {
      req = holder.template get_params<params_json>();
    }
    catch (const json::json_error& e)
    {
      if ( holder.has_id() )
      {
        TT::template send_error<T, error_json>( 
          std::move(holder), 
          std::make_unique<invalid_params>(), 
          std::move(outgoing_handler) 
        );
      }
      COMMON_LOG_ERROR("jsonrpc::invoke Invalid Params: " << holder.params_error_message(e) )
    }
      
    try // server_error
    {
      if ( holder.is_notify() )
      {
        Handler::operator()( t, std::move(req), nullptr);
      }
      else
      {
        auto ph = std::make_shared<incoming_holder>( std::move(holder) );
        Handler::operator()( t, std::move(req), 
          [ph, outgoing_handler]( result_ptr params, error_ptr err )
          {
            try
            {
              if (err == nullptr )
              {
                TT::template send_result<T, result_json>( 
                  std::move(*ph), 
                  std::move(params), 
                  std::move(outgoing_handler) 
                );
              }
              else
              {
                TT::template send_error<T, error_json>( 
                  std::move(*ph), 
                  std::move(err), 
                  std::move(outgoing_handler)
                );
              }
            }
            catch(const std::exception& e)
            {
              DAEMON_LOG_FATAL("jsonrpc service exception: " << e.what() )
              abort();
            }
            catch(...)
            {
              DAEMON_LOG_FATAL("jsonrpc service unhandled exception")
              abort();
            }
          } // callback 
        );
      }
    }
    catch(const std::exception& e)
    {
      DAEMON_LOG_ERROR("wfc::jsonrpc::invoke::operator() : " << e.what() )
      // Ахтунг! holder перемещен
      TT::template send_error<T, error_json>( 
        std::move(holder), 
        std::make_unique<server_error>(),
        std::move(outgoing_handler) 
      );
      throw;
    }
    catch(...)
    {
      DAEMON_LOG_ERROR("wfc::jsonrpc::invoke::operator() : unhandled exception (Server Error)")
      // Ахтунг! holder перемещен
      TT::template send_error<T, error_json>( 
        std::move(holder), 
        std::make_unique<server_error>(),
        std::move(outgoing_handler) 
      );
      throw;
    }
  }

};


}} // wfc


