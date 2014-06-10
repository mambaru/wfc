#pragma once

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
template<typename Impl>
struct method_impl  
  : fas::type_list< Impl, 
    fas::type_list< fas::group<_method_, typename Impl::tag>
    > > 
{
};

}}
