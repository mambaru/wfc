#pragma once

#include <wfc/jsonrpc/method/basic_method.hpp>
#include <fas/aop.hpp>
#include <fas/type_list.hpp>

namespace wfc{ namespace jsonrpc{
  

template< typename... Args>
struct method
  : fas::type_list< basic_method< fas::aspect<Args...> >, 
    fas::type_list< fas::group<_method_, typename basic_method< fas::aspect<Args...> >::tag>
    > > 
{};


}}
