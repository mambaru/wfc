#pragma once


#include <wfc/jsonrpc/handler/invoke.hpp>
#include <wfc/jsonrpc/handler/mem_fun_handler.hpp>

#include <wfc/callback/callback_status.hpp>
#include <memory>

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
  : invoke<JReq, JResp, mem_fun_handler<typename JReq::target, typename JResp::target, Target, mem_ptr> >
{};

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
struct invoke_mem_fun_ex
  : invoke<JReq, JResp, mem_fun_handler_ex<typename JReq::target, typename JResp::target, Target, Itf, mem_ptr> >
{};

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


