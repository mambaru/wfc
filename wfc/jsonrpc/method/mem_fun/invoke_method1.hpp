#pragma once

#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/mem_fun/invoke_mem_fun1.hpp>
#include <wfc/jsonrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace wfc{ namespace jsonrpc{

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
struct invoke_method1_basic: method_basic< 
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

}} // wfc


