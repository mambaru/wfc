#pragma once

#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/mem_fun/invoke_mem_fun.hpp>
#include <wfc/jsonrpc/method/method.hpp>
#include <memory>
#include <functional>


namespace wfc{ namespace jsonrpc{

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

}} // wfc


