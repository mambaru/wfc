#pragma once

#include <wfc/io/reader/common/aspect.hpp>
#include <wfc/io/reader/errors/aspect.hpp>
#include <wfc/io/reader/stream/async/aspect.hpp>
#include <wfc/io/reader/loop/aspect.hpp>
#include <wfc/io/reader/tags.hpp>

#include <wfc/io/context.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace reader{ 

typedef fas::type_list_n<
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list,
  common::advice_list,
  errors::advice_list,
  stream::async::advice_list,
  fas::alias< loop::_read_some_, stream::async::_async_read_some_>,
  fas::alias< stream::async::_incoming_, loop::_ready_ >,
  fas::alias< loop::_incoming_, _incoming_ >,
  loop::advice_list
>
{};

}}}
