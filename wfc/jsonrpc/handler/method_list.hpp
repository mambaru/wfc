#pragma once

#include <wfc/jsonrpc/handler/handler_base.hpp>
#include <wfc/jsonrpc/handler/handler_aspect.hpp>
#include <wfc/jsonrpc/handler/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class method_list
  : public handler_base
  , public AspectClass< typename fas::merge_aspect<A, handler_aspect>::type >
  , public std::enable_shared_from_this< method_list<A, AspectClass> >
{
public:
  typedef method_list<A, AspectClass> self;
  typedef self magic;
  typedef AspectClass< typename fas::merge_aspect<A, handler_aspect>::type > super;
  typedef typename super::aspect::template advice_cast<_target_>::type target_type;
};


}} // wfc


