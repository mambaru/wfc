#pragma once


#include <wfc/jsonrpc/handler/tags.hpp>
#include <fas/aop.hpp>
#include <fas/type_list.hpp>

namespace wfc{ namespace jsonrpc{

template< typename Method>
struct add_method: fas::type_list<
  Method,
  fas::type_list<
    fas::group<_method_, typename Method::tag>
  >
> {};


  /*
template< typename Method>
struct add_method: fas::type_list_n<
  Method,
  fas::group<_method_, typename Method::tag>
>::type {};
*/


}} // wfc


