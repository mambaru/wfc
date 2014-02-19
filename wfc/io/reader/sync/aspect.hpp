#pragma once

#include <wfc/io/reader/sync/ad_read_some.hpp>
#include <wfc/io/reader/sync/tags.hpp>


namespace wfc{ namespace io{ namespace reader{ namespace sync{ 

typedef fas::type_list_n<
  fas::advice< _read_some_, ad_read_some>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::stub<_ready_>
>
{};

}}}}
