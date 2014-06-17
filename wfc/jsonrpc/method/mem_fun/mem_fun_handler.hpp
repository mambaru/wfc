#pragma once

#include <wfc/jsonrpc/method/mem_fun/mem_fun_helper.hpp>

namespace wfc{ namespace jsonrpc{

template<
  typename Params, 
  typename Result, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<Params>, 
    std::function< void(std::unique_ptr<Result>) > 
  ) 
>
struct mem_fun_handler
{
  typedef std::unique_ptr<Params> request_ptr;
  typedef std::unique_ptr<Result> responce_ptr;
  typedef std::unique_ptr< ::wfc::jsonrpc::error> json_error_ptr;
  typedef std::function< void(responce_ptr, json_error_ptr) > jsonrpc_callback;

  template<typename T>
  void operator()(T& t, request_ptr req, jsonrpc_callback cb) const
  {
    if ( auto i = t.target().lock() )
    {
      (i.get()->*mem_ptr)( 
        std::move(req), 
        mem_fun_make_callback( std::move(cb)) 
      );
    }
    else 
    {
      mem_fun_service_unavailable( std::move(cb) );
    }
  }
};

}} // wfc


