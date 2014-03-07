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
  void operator()(T& t, incoming_holder holder)
  {
    std::cout << "METHOD!" << std::endl;
    this->get_aspect().template get<_invoke_>()(t, std::move(holder));
  }
  
  template<typename T, typename ReqPtr, typename Callback>
  void call(T& t, ReqPtr req, Callback callback)
  {
    this->get_aspect().template get<_call_>()( t, *this, std::move(req), callback);
  }
};

template< typename... Args>
struct method: fas::type_list_n<
  method_impl<Args...>,
  fas::group<_method_, typename method_impl<Args...>::tag>
>::type {};


}} // wfc


