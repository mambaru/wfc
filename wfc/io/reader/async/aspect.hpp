#pragma once

#include <wfc/io/reader/async/ad_async_read_some.hpp>
#include <wfc/io/reader/async/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace async{ 

typedef fas::type_list_n<
  fas::advice< _async_read_some_, ad_async_read_some>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::stub<_ready_>
>
{};

}}}}
