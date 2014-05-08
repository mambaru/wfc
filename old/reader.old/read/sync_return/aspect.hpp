#pragma once

#include <wfc/io/reader/read/sync_return/ad_read.hpp>
#include <wfc/io/reader/read/sync_return/ad_incoming.hpp>
#include <wfc/io/reader/read/sync_return/tags.hpp>

#include <wfc/io/reader/errors/aspect.hpp>
#include <wfc/io/reader/stream/sync/aspect.hpp>
#include <wfc/io/reader/common/aspect.hpp>
#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>

#include <fas/aop.hpp>
#include <list>

namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace sync_return{ 

// TODO: сделать mutex

typedef std::list< basic::data_ptr> data_list;

typedef fas::type_list_n<
  fas::value<  _incoming_list_, data_list>,
  fas::advice< _read_, ad_read>,
  fas::advice< _incoming_, ad_incoming >
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  //fas::alias< common::_outgoing_, _incoming_ >,
  fas::alias< wfc::io::reader::_read_, _read_ >
>
{};

}}}}}
