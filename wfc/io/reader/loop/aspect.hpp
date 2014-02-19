#pragma once

#include <wfc/io/reader/async/loop/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace loop{  

typedef fas::type_list_n<
  
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
