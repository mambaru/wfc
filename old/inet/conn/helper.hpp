#pragma once
#include <fas/aop.hpp>
#include <wfc/inet/tags.hpp>
#include <wfc/inet/conn/tags.hpp>

namespace wfc{ namespace inet{ namespace conn{
  
template<typename A, template<typename> class AspectClass >
struct helper
{
  typedef AspectClass<A> base_class;
  typedef typename base_class::aspect::template advice_cast< wfc::inet::_socket_type_>::type socket_type;
  typedef typename base_class::aspect::template advice_cast< wfc::inet::_context_ >::type context_type;
  /*
  typedef typename base_class::aspect::template advice_cast<_context_>::type user_context;
  typedef typename base_class::aspect::template advice_cast<_basic_context_>::type
                   ::template apply<user_context>::type context_type;
  */
};

}}}
