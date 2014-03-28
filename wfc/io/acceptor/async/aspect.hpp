#pragma once

#include <wfc/io/acceptor/async/ad_async_accept.hpp>
#include <wfc/io/acceptor/async/tags.hpp>

namespace wfc{ namespace io{ namespace acceptor{ namespace async{ 

typedef fas::type_list_n<
  fas::advice< _async_accept_, ad_async_accept>,
  fas::alias< _incoming_, _async_accept_ >
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
