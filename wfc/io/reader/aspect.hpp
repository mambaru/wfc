#pragma once

#include <wfc/io/reader/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 

typedef fas::type_list_n<
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}
