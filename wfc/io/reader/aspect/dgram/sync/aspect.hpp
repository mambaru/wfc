#pragma once


#include <wfc/io/reader/aspect/dgram/sync/ad_receive_from.hpp>
#include <wfc/io/reader/aspect/dgram/sync/ad_user_handler.hpp>
#include <wfc/io/reader/aspect/dgram/sync/ad_init.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace dgram{ namespace sync{

struct aspect: fas::aspect<
  fas::advice< _init_, ad_init >,
  fas::advice< _read_some_,    ad_receive_from >,
  fas::advice< _user_handler_, ad_user_handler>
>{};



}}}}}
