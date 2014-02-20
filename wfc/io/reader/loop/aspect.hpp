#pragma once

#include <wfc/io/reader/loop/ad_more.hpp>
#include <wfc/io/reader/loop/ad_ready.hpp>
#include <wfc/io/reader/loop/ad_start.hpp>
#include <wfc/io/reader/loop/tags.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace loop{  

typedef fas::type_list_n<
  fas::advice<_more_, ad_more>,
  fas::advice<_ready_, ad_ready>,
  fas::advice<_start_, ad_start>,
  fas::group< wfc::io::_start_, _start_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
