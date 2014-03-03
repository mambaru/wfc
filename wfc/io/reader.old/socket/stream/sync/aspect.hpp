#pragma once

#include <wfc/io/reader/sync/ad_read_some.hpp>
#include <wfc/io/reader/sync/ad_ready.hpp>
#include <wfc/io/reader/sync/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace sync{ 

typedef fas::type_list_n<
  fas::advice< _read_some_, ad_read_some>,
  fas::advice< _ready_, ad_ready>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::alias< _incoming_, wfc::io::reader::_incoming_ >
>
{};

}}}}
