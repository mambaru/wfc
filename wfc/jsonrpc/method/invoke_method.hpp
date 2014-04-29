#pragma once

#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/mem_fun.hpp>
#include <wfc/jsonrpc/method/method.hpp>
#include <memory>
#include <wfc/jsonrpc/method/aspect/call.hpp>

namespace wfc{ namespace jsonrpc{

#warning осталось только этот файл 
template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename I, 
  void (I::*mem_ptr)( std::unique_ptr<typename JParams::target>, std::function< void(std::unique_ptr<typename JResult::target>) > ) 
>
struct invoke_method_basic: method_basic< 
  name<TgName>,
  invoke_mem_fun< 
    JParams,
    JResult,
    I,
    mem_ptr
  >
>
{};

template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename I, 
  void (I::*mem_ptr)( std::unique_ptr<typename JParams::target>, std::function< void(std::unique_ptr<typename JResult::target>) > ) 
>
struct invoke_method: method< 
  name<TgName>,
  invoke_mem_fun< 
    JParams,
    JResult,
    I,
    mem_ptr
  >
>
{};



template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename Target, 
  typename Itf, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t, 
    std::weak_ptr<Itf>
  )
>
struct invoke_method1: method< 
  name<TgName>,
  invoke_mem_fun1< 
    JParams,
    JResult,
    Target,
    Itf, 
    mem_ptr
  >
>
{};



template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  ) 
>
struct dual_method: method< 
  name<TgName>,
  invoke_mem_fun<JParams,JResult,I,mem_ptr>,
  call<JParams, JResult>
>
{};

template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  ) 
>
struct dual_method_basic: method_basic< 
  name<TgName>,
  invoke_mem_fun<JParams,JResult,I,mem_ptr>,
  call<JParams, JResult>
>
{};


/*
template<typename Method>
struct add_method: fas::type_list_n<
  Method,
  fas::group<_method_, typename Method::tag>
>::type {};
*/

template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t, 
    std::weak_ptr<Target>
  ) 
>
struct dual_method1: method< 
  name<TgName>,
  invoke_mem_fun1< JParams, JResult, Target, Target, mem_ptr>,
  call<JParams, JResult>
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

template<
  typename TgName, 
  typename JReq, 
  typename JResp, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JReq::target>, 
    std::function< void(std::unique_ptr<typename JResp::target>) >, 
    size_t
  )
>
struct dual_method3: method< 
  name<TgName>,
  invoke_mem_fun3< JReq, JResp, Target, mem_ptr>,
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
struct invoke_method2: method< 
  name<TgName>,
  invoke_mem_fun2< JReq, JResp, Req2, Resp2, Target, mem_ptr, Itf, mem_ptr2>
>
{};

template<
  typename TgName, 
  typename JReq, 
  typename JResp, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JReq::target>, 
    std::function< void(std::unique_ptr<typename JResp::target>) >, 
    size_t
  )
>
struct invoke_method3: method< 
  name<TgName>,
  invoke_mem_fun3< JReq, JResp, Target, mem_ptr>
>
{};



}} // wfc


