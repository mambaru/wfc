#pragma once

#include <wfc/io/reader/common/trace/ad_trace.hpp>
#include <wfc/io/reader/common/trace/tags.hpp>
#include <wfc/io/reader/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace common{ namespace trace{

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
