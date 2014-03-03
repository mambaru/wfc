#pragma once

#include <wfc/io/reader/stream/sync/ad_read_some.hpp>
//#include <wfc/io/reader/stream/sync/ad_ready.hpp>
#include <wfc/io/reader/stream/sync/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace stream{ namespace sync{ 

typedef fas::type_list_n<
  fas::advice< _read_some_, ad_read_some>,
  fas::alias< _incoming_, _read_some_ >
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}}
