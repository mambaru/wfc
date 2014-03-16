#pragma once

#include <wfc/io/writer/stream/sync/ad_write_some.hpp>
#include <wfc/io/writer/stream/sync/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace stream{ namespace sync{ 

typedef fas::type_list_n<
  fas::advice< _write_some_, ad_write_some>,
  fas::alias< _incoming_, _write_some_ >
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}}
