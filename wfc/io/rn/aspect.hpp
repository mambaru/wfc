#pragma once

#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/rn/writer/aspect.hpp>

namespace wfc{ namespace io{ namespace rn{

typedef fas::type_list_n<
  reader::advice_list,
  writer::advice_list
>::type advice_list;
  
struct aspect: fas::aspect< 
  advice_list,
  fas::alias< reader::_outgoing_, writer::_incoming_>
>
{};

}}}
