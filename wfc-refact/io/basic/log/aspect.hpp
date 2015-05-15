#pragma once

#include <wfc/io/basic/log/ad_create.hpp>
#include <wfc/io/basic/log/tags.hpp>
#include <wfc/io/basic/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace basic{ namespace log{

typedef fas::type_list_n<
  fas::advice<_create_, ad_create>,
  fas::group<wfc::io::_create_, _create_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
