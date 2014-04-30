#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/method/aspect_method.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

template< typename... Args >
class method_basic
  : public fas::aspect_class< typename fas::merge_aspect<fas::aspect<Args...>, aspect_method>::type >
{
public:
  
  typedef method_basic<Args...> self;
  typedef fas::aspect_class< typename fas::merge_aspect<fas::aspect<Args...>, aspect_method>::type > super;
  
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
  void operator()(T& t, incoming_holder holder, ::wfc::io::outgoing_handler_t callback) const
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
struct method: fas::type_list<
  method_basic<Args...>,
  fas::type_list<
    fas::group<_method_, typename method_basic<Args...>::tag>
  >
> {};
}}


