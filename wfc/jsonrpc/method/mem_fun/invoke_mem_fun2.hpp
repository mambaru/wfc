#pragma once

#include <wfc/jsonrpc/method/aspect/invoke.hpp>
#include <wfc/jsonrpc/method/mem_fun/mem_fun_handler2.hpp>

namespace wfc{ namespace jsonrpc{
 
template<
  typename JReq, 
  typename JResp, 
  typename Req2, 
  typename Resp2, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JReq::target>, 
    std::function< void(std::unique_ptr<typename JResp::target>) >, 
    size_t, 
    std::function< void(
      std::unique_ptr<Req2>, 
      std::function< void(std::unique_ptr<Resp2>) >  
    ) >
  ),
  typename Itf,
  void (Itf::*mem_ptr2)( 
    std::unique_ptr<Req2>, 
    std::function< void(std::unique_ptr<Resp2>) >
  )
>
struct invoke_mem_fun2
  : invoke<
      JReq, 
      JResp, 
      mem_fun_handler2<
        typename JReq::target, 
        typename JResp::target,
        Req2,
        Resp2,
        Target, 
        mem_ptr,
        Itf,
        mem_ptr2
      > 
    >
{};



}} // wfc


