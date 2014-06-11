#pragma once

#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/mem_fun/invoke_mem_fun3.hpp>
#include <wfc/jsonrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace wfc{ namespace jsonrpc{

template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t
  )
>
struct basic_invoke_method3: basic_method< 
  name<TgName>,
  invoke_mem_fun3< JParams, JResult, Target, mem_ptr>
>
{};


template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t
  )
>
struct invoke_method3: method< 
  name<TgName>,
  invoke_mem_fun3< JParams, JResult, Target, mem_ptr>
>
{};


}} // wfc


