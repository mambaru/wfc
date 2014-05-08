#pragma once

#include <wfc/jsonrpc/method/aspect/invoke.hpp>
#include <wfc/jsonrpc/method/mem_fun/mem_fun_handler1.hpp>

namespace wfc{ namespace jsonrpc{
  
template<
  typename JReq, 
  typename JResp, 
  typename Target, 
  typename Itf,
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JReq::target>, 
    std::function< void(std::unique_ptr<typename JResp::target>) >, 
    size_t, 
    std::weak_ptr<Itf>
  ) 
>
struct invoke_mem_fun1
  : invoke<
      JReq, 
      JResp, 
      mem_fun_handler1<
        typename JReq::target, 
        typename JResp::target, 
        Target, 
        Itf, 
        mem_ptr
      > 
    >
{
  
};

}} // wfc


