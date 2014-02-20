#pragma once

#include <wfc/io/rn/writer/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace writer{

typedef fas::type_list_n<
>::type advice_list;
  
struct aspect: fas::aspect< 
  advice_list
>
{};

}}}}
