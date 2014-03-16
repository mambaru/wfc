#pragma once

#include <wfc/io/writer/errors/ad_aborted.hpp>
#include <wfc/io/writer/errors/ad_error.hpp>
#include <wfc/io/writer/errors/tags.hpp>


namespace wfc{ namespace io{ namespace writer{ namespace errors{ 

typedef fas::type_list_n<
  fas::advice< _aborted_, ad_aborted>,
  fas::advice< _error_, ad_error>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
