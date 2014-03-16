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




}} // wfc


