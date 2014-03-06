#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/handler/method_aspect.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

template< typename... Args >
class method
  : public fas::aspect_class< typename fas::merge_aspect<fas::aspect<Args...>, method_aspect>::type >
{
public:
  
  typedef method<Args...> self;
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
};

}} // wfc


