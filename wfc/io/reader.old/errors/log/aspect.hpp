#pragma once

#include <wfc/io/reader/errors/log/ad_aborted.hpp>
#include <wfc/io/reader/errors/log/ad_error.hpp>
#include <wfc/io/reader/errors/log/tags.hpp>
#include <wfc/io/reader/errors/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace errors{ namespace log{

typedef fas::type_list_n<
  fas::advice<_error_, ad_error>,
  fas::advice<_aborted_, ad_aborted>,
  fas::group<_on_error_, _error_>,
  fas::group<_on_aborted_, _aborted_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}}
