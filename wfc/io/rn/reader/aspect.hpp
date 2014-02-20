#pragma once

#include <wfc/io/rn/reader/tags.hpp>
#include <wfc/io/rn/reader/ad_splitter.hpp>
#include <wfc/io/rn/reader/ad_trimmer.hpp>
#include <wfc/io/rn/reader/ad_ready.hpp>
#include <wfc/io/rn/rn.hpp>
#include <wfc/io/basic/types.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace reader{

typedef fas::type_list_n<
  fas::alias<_incoming_, _splitter_>,
  fas::advice<_splitter_, ad_splitter>,
  fas::advice<_trimmer_,  ad_trimmer>,
  fas::advice<_ready_,    ad_ready>,
  fas::advice<_separator_, rn>,
  fas::value<_buffer_, basic::data_ptr>
>::type advice_list;
  
struct aspect: fas::aspect< 
  fas::stub<_check_>,
  advice_list
>
{};

}}}}
