#pragma once

#include <wfc/io/rn/reader/trace/ad_trace.hpp>
#include <wfc/io/rn/reader/trace/tags.hpp>
#include <wfc/io/rn/reader/tags.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace reader{ namespace trace{

typedef fas::type_list_n<
  fas::advice<_trace_, ad_trace>,
  fas::group<_on_read_, _trace_>
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}}
