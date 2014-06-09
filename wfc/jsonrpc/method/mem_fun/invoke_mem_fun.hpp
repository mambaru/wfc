#pragma once

#include <wfc/jsonrpc/method/aspect/invoke.hpp>
#include <wfc/jsonrpc/method/mem_fun/mem_fun_handler.hpp>

namespace wfc{ namespace jsonrpc{
  
template<
  typename JReq, 
  typename JResp, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JReq::target>, 
    std::function< void(std::unique_ptr<typename JResp::target>) > 
  ) 
>
struct invoke_mem_fun
  : invoke<
      JReq, 
      JResp, 
      mem_fun_handler<
        typename JReq::target, 
        typename JResp::target, 
        Target, 
        mem_ptr
      >
    >
{};

}} // wfc


