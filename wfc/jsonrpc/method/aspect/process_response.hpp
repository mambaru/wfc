#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/logger.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
struct process_response
  : fas::type<_process_response_, process_response>
{
  template<typename T, typename JResult, typename JError>
  static inline void process(
    typename T::holder_type holder, 
    std::function< void (
      std::unique_ptr<typename JResult::target>, 
      std::unique_ptr<typename JError::target>
    )> callback
  ) 
  {
    // получатель
    if ( holder.is_response() )
    {
      try
      {
        auto pres = holder.template get_result<JResult>();
        callback( std::move(pres), nullptr);
      }
      catch(::wfc::json::json_error& e)
      {
        COMMON_LOG_ERROR("wfc::jsonrpc::process_response (result): json exception: " << e.what() << std::endl << holder.result_error_message(e) );
        callback( nullptr, nullptr);
      }
      catch(std::exception& e)
      {
        COMMON_LOG_ERROR("wfc::jsonrpc::process_response (result): exception: " << e.what());
      }
      catch(...)
      {
        COMMON_LOG_ERROR("wfc::jsonrpc::process_response (result): unhandled exception");
      }
    }
    else if ( holder.is_error() )
    {
      try
      {
        auto perr = holder.template get_error<JError>();
        callback( nullptr, std::move(perr) );
      }
      catch(::wfc::json::json_error& e)
      {
        COMMON_LOG_ERROR("wfc::jsonrpc::process_response (error): json exception: " << e.what() << std::endl << holder.result_error_message(e) );
        callback( nullptr, nullptr);
      }
      catch(std::exception& e)
      {
        COMMON_LOG_ERROR("wfc::jsonrpc::process_response (error): exception: " << e.what());
      }
      catch(...)
      {
        COMMON_LOG_ERROR("wfc::jsonrpc::process_response (error): unhandled exception");
      }
    }
    else
    {
      callback( nullptr, nullptr);
    }
  }
};

struct process_response_proxy
  : fas::type<_process_response_, process_response_proxy>
{
  template<typename T, typename JResult, typename JError>
  static inline void process(
    typename T::holder_type holder, 
    std::function< void (
      std::unique_ptr<typename JResult::target>, 
      std::unique_ptr<typename JError::target>
    )> callback
  ) 
  {
    return T::aspect::template advice_cast<_process_response_>::type
            ::template process<T, JResult, JError>( 
                std::move(holder), 
                std::move(callback) 
              );
  }
};


}} // wfc


