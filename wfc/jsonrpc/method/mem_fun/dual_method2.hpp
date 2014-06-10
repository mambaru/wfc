#pragma once

#include <wfc/jsonrpc/method/aspect/call.hpp>
#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/mem_fun/invoke_mem_fun2.hpp>
#include <wfc/jsonrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace wfc{ namespace jsonrpc{

template<
  typename TgName, 
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
struct basic_dual_method2: basic_method< 
  name<TgName>,
  invoke_mem_fun2< JReq, JResp, Req2, Resp2, Target, mem_ptr, Itf, mem_ptr2>,
  call<JReq, JResp>
>
{};

  
template<
  typename TgName, 
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
struct dual_method2: method< 
  name<TgName>,
  invoke_mem_fun2< JReq, JResp, Req2, Resp2, Target, mem_ptr, Itf, mem_ptr2>,
  call<JReq, JResp>
>
{};


}} // wfc


