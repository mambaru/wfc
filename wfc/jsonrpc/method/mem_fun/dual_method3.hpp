#pragma once

#include <wfc/jsonrpc/method/aspect/call.hpp>
#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/mem_fun/invoke_mem_fun3.hpp>
#include <wfc/jsonrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace wfc{ namespace jsonrpc{

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
struct dual_method3_basic: method_basic< 
  name<TgName>,
  invoke_mem_fun3< JReq, JResp, Target, mem_ptr>,
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

  
}} // wfc


