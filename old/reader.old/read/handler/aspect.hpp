#pragma once

#include <wfc/io/reader/read/handler/ad_read.hpp>
#include <wfc/io/reader/read/handler/tags.hpp>
#include <wfc/io/reader/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace handler{

typedef fas::type_list_n<
  fas::advice<_read_, ad_read>,
  fas::alias< _incoming_, _read_ >
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::alias< wfc::io::reader::_read_, _read_ >
  
>
{};

}}}}}
