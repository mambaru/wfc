#pragma once

#include <wfc/io/rn/reader/check/ad_check.hpp>
#include <wfc/io/rn/reader/check/ad_create.hpp>
#include <wfc/io/rn/reader/check/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace reader{ namespace check{

typedef fas::type_list_n<
  fas::advice<_check_,    ad_check>,
  fas::advice<_create_,   ad_create>,
  fas::value<_limit_warning_, size_t>,
  fas::value<_limit_error_, size_t>,
  fas::group<wfc::io::_create_, _create_>
>::type advice_list;
  
struct aspect: fas::aspect< 
  advice_list
>
{};

}}}}}
