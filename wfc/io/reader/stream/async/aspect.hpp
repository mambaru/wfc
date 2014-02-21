#pragma once

#include <wfc/io/reader/stream/async/ad_async_read_some.hpp>
//#include <wfc/io/reader/stream/async/ad_ready.hpp>
#include <wfc/io/reader/stream/async/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace stream{ namespace async{ 

typedef fas::type_list_n<
  fas::advice< _async_read_some_, ad_async_read_some>,
  fas::alias< common::_reader_, _async_read_some_>
  /*,
  fas::advice< _ready_, ad_ready>*/
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::alias< _incoming_, wfc::io::reader::_incoming_ >
>
{};

}}}}}
