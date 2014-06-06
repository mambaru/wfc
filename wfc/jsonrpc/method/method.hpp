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
  
  template<typename T, typename J>
  static void invoke_result(typename T::holder_type holder, std::unique_ptr<typename J::target> result, typename T::outgoing_handler_t outgoing_handler)
  {
    super::aspect::template advice_cast<_invoke_result_>::type::template send_result<T, J>( std::move(holder), std::move(result), std::move(outgoing_handler) );
  }

  template<typename T, typename J>
  static void invoke_error(typename T::holder_type holder, std::unique_ptr<typename J::target> err, typename T::outgoing_handler_t outgoing_handler)
  {
    super::aspect::template advice_cast<_invoke_error_>::type::template send_error<T, J>( std::move(holder), std::move(err), std::move(outgoing_handler) );
  }
  
  template<typename T>
  void operator()(T& t, typename T::holder_type holder, typename T::outgoing_handler_t outgoing_handler) 
  {
    this->get_aspect().template get<_invoke_>()(t, *this, std::move(holder), std::move(outgoing_handler) );
  }
  
  template<typename T, typename ReqPtr, typename Callback>
  void call(T& t, ReqPtr req, Callback callback) const
  {
    this->get_aspect().template get<_call_>()( t, *this, std::move(req), std::move(callback) );
  }
};

template< typename... Args>
struct method: fas::type_list
<
  method_basic<Args...>, fas::type_list<
  fas::group<_method_, typename method_basic<Args...>::tag>
> > 
{};

}}

