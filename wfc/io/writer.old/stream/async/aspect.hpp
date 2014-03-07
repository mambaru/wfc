#pragma once

#include <wfc/io/writer/stream/async/ad_async_write_some.hpp>
#include <wfc/io/writer/stream/async/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace stream{ namespace async{ 

typedef fas::type_list_n<
  fas::advice< _async_write_some_, ad_async_write_some>,
  fas::alias< _incoming_, _async_write_some_ >
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}}
