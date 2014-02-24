#pragma once


#include <wfc/io/rn/reader/log/ad_limit_error.hpp>
#include <wfc/io/rn/reader/log/ad_limit_warning.hpp>
#include <wfc/io/rn/reader/log/tags.hpp>
#include <wfc/io/rn/reader/check/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace reader{ namespace log{

typedef fas::type_list_n<
  fas::advice<_limit_error_, ad_limit_error>,
  fas::advice<_limit_warning_, ad_limit_warning>,
  fas::group< check::_on_limit_error_, _limit_error_>,
  fas::group< check::_on_limit_warning_, _limit_warning_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}}
