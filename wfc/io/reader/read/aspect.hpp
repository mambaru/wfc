#pragma once

#include <wfc/io/reader/read/ad_read.hpp>
#include <wfc/io/reader/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace read{ 

typedef fas::type_list_n<
  fas::advice<_read_, ad_read>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
