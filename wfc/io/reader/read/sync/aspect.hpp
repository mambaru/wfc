#pragma once

#include <wfc/io/reader/read/sync/ad_read.hpp>
#include <wfc/io/reader/read/sync/ad_ready.hpp>
#include <wfc/io/reader/read/sync/tags.hpp>

#include <wfc/io/reader/errors/aspect.hpp>
#include <wfc/io/reader/sync/aspect.hpp>
#include <wfc/io/reader/common/aspect.hpp>
#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>

#include <fas/aop.hpp>
#include <list>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace sync{ 

// TODO: сделать mutex

typedef std::list< basic::data_ptr> data_list;

typedef fas::type_list_n<
  fas::value<_incoming_list_, data_list>,
  fas::advice<_read_, ad_read>,
  fas::advice< wfc::io::reader::sync::_ready_, ad_ready >
>::type advice_list;


struct aspect: fas::aspect
< 
  context<>,
  advice_list,
  wfc::io::reader::sync::advice_list,
  wfc::io::reader::errors::advice_list,
  wfc::io::reader::common::advice_list, 
  posix::advice_list,
  basic::advice_list
    /*,
  fas::type< wfc::io::_config_type_, basic::config>,
  fas::type< wfc::io::_init_type_, posix::init >*/
>
{};

}}}}}
