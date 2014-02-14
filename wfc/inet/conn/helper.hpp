#pragma once
#include <fas/aop.hpp>
#include <wfc/inet/conn/tags.hpp>

namespace wfc{ namespace inet{ namespace conn{
  
template<typename A, template<typename> class AspectClass >
struct helper
{
  /*
  typedef typename fas::aspect_class< A >::aspect original_aspect;
  typedef typename original_aspect::template advice_cast<_context_>::type original_context;
  typedef typename original_aspect::template advice_cast<_basic_context_>::type::template apply<original_context>::type context_type;
  typedef typename fas::merge_aspect<
    context<context_type>,
    A
  >::type new_aspect;
  typedef AspectClass<new_aspect> base_class;
  */
  
  typedef AspectClass<A> base_class;
  typedef typename base_class::aspect::template advice_cast<_socket_type_>::type socket_type;
  typedef typename base_class::aspect::template advice_cast<_context_>::type user_context;
  typedef typename base_class::aspect::template advice_cast<_basic_context_>::type
                   ::template apply<user_context>::type context_type;
  
};

}}}
