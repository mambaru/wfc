#pragma once

#include <wfc/io/writer/loop/ad_more.hpp>
#include <wfc/io/writer/loop/ad_ready.hpp>
#include <wfc/io/writer/loop/tags.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace loop{

typedef fas::type_list_n<
  fas::advice<_more_, ad_more>,
  fas::advice<_ready_, ad_ready>,
  fas::alias<_incoming_, _ready_>
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
