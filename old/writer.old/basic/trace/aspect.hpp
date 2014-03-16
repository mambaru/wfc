#pragma once

#include <wfc/io/writer/basic/trace/ad_trace.hpp>
#include <wfc/io/writer/basic/trace/tags.hpp>
#include <wfc/io/writer/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace basic{ namespace trace{

typedef fas::type_list_n<
  fas::advice<_trace_, ad_trace>,
  fas::group<_on_write_, _trace_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}}
