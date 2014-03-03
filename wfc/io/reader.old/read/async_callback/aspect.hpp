#pragma once

#include <wfc/io/reader/read/async_callback/ad_read.hpp>
#include <wfc/io/reader/read/async_callback/ad_incoming.hpp>
#include <wfc/io/reader/read/async_callback/tags.hpp>
#include <wfc/io/reader/errors/aspect.hpp>
#include <wfc/io/reader/stream/async/aspect.hpp>
#include <wfc/io/reader/common/aspect.hpp>

#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>


#include <functional>
#include <list>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace async_callback{ 

typedef std::function<void(basic::data_ptr)> callback_type;
typedef std::list< basic::data_ptr> data_list;
typedef std::list<callback_type> callback_list;

typedef fas::type_list_n<
  fas::value<_incoming_list_, data_list>,
  fas::value<_callback_list_, callback_list>,
  fas::advice< _read_, ad_read>,
  fas::advice< _incoming_, ad_incoming>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::alias< wfc::io::reader::_read_, _read_ >
>
{};

}}}}}
