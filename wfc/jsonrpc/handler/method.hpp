#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/handler/method_aspect.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

template< typename... Args >
class method_impl
  : public fas::aspect_class< typename fas::merge_aspect<fas::aspect<Args...>, method_aspect>::type >
{
public:
  
  typedef method_impl<Args...> self;
  typedef fas::aspect_class< typename fas::merge_aspect<fas::aspect<Args...>, method_aspect>::type > super;
  
  typedef fas::metalist::advice metatype;
  typedef typename super::aspect::template advice_cast<_name_>::type::name_type tag;
  typedef self advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  const char* name() const
  {
    return this->get_aspect().template get<_name_>()();
  }

  template<typename T>
  void operator()(T& t, incoming_holder holder, ::wfc::io::callback callback) const
  {
    this->get_aspect().template get<_invoke_>()(t, std::move(holder), callback);
  }
  
  template<typename T, typename ReqPtr, typename Callback>
  void call(T& t, ReqPtr req, Callback callback) const
  {
    this->get_aspect().template get<_call_>()( t, *this, std::move(req), callback);
  }
};

template< typename... Args>
struct method: fas::type_list_n<
  method_impl<Args...>,
  fas::group<_method_, typename method_impl<Args...>::tag>
>::type {};
}}

#include <wfc/jsonrpc/handler/name.hpp>
#include <wfc/jsonrpc/handler/invoke_mem_fun.hpp>
#include <wfc/jsonrpc/handler/method.hpp>
#include <memory>


#include <wfc/jsonrpc/handler/call.hpp>
#include <wfc/jsonrpc/handler/target.hpp>

namespace wfc{ namespace jsonrpc{

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
  typename I, 
  void (I::*mem_ptr)( std::unique_ptr<typename JParams::target>, std::function< void(std::unique_ptr<typename JResult::target>) > ) 
>
struct invoke_method_basic: method_impl< 
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
struct invoke_method_ex: method< 
  name<TgName>,
  invoke_mem_fun_ex< 
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
struct dual_method_basic: method_impl< 
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
struct dual_method_ex: method< 
  name<TgName>,
  invoke_mem_fun_ex< JParams, JResult, Target, Target, mem_ptr>,
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


template<
  typename Interface,
  typename Target, 
  void (Target::*mem_ptr1)( size_t, std::weak_ptr<Interface> ),
  void (Target::*mem_ptr2)( size_t/*, std::weak_ptr<Interface>*/ )
>
struct interface_target_ctl: fas::type_list_n<
    interface_<Interface>, 
    target<Target>, 
    startup< mem_fun_startup<Interface, Target, mem_ptr1> >, 
    shutdown< mem_fun_shutdown< Target, mem_ptr2> >
>::type {};

}} // wfc


