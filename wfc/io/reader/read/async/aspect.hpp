#pragma once

#include <wfc/io/reader/read/async/ad_read.hpp>
#include <wfc/io/reader/read/async/ad_incoming.hpp>
#include <wfc/io/reader/read/async/tags.hpp>
#include <wfc/io/reader/errors/aspect.hpp>
#include <wfc/io/reader/async/aspect.hpp>
#include <wfc/io/reader/common/aspect.hpp>

#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>


#include <functional>
#include <list>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace async{ 

typedef std::function<void(basic::data_ptr)> callback_type;
typedef std::list< basic::data_ptr> data_list;
typedef std::list<callback_type> callback_list;

typedef fas::type_list_n<
  fas::value<_incoming_list_, data_list>,
  
  fas::value<_callback_list_, callback_list>,
  fas::advice< _read_, ad_read>,
  fas::advice< wfc::io::reader::async::_incoming_, ad_incoming >
>::type advice_list;


struct aspect: fas::aspect
< 
  context<>,
  advice_list,
  posix::advice_list,
  basic::advice_list,
  wfc::io::reader::async::advice_list,
  wfc::io::reader::errors::advice_list,
  wfc::io::reader::common::advice_list/*,
  fas::type< wfc::io::_config_type_, basic::config>,
  fas::type< wfc::io::_init_type_, posix::init >*/
>
{};

}}}}}
