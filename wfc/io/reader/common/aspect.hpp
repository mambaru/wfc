#pragma once

#include <wfc/io/reader/common/ad_make_buffer.hpp>
#include <wfc/io/reader/common/ad_create.hpp>
#include <wfc/io/reader/common/tags.hpp>
#include <wfc/io/tags.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace reader{  namespace common{

typedef fas::type_list_n<
  fas::value< _input_buffer_size_, size_t>,
  fas::advice<_make_buffer_, ad_make_buffer>,
  fas::advice<_create_, ad_create>,
  fas::group< wfc::io::_create_, _create_>
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list
>
{};

}}}}
