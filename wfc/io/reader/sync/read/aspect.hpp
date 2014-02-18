#pragma once

#include <wfc/io/reader/sync/read/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace sync{ namespace read{

typedef fas::type_list_n<
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}}
