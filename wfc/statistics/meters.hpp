//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wrtstat/meters/multi_meter.hpp>
#include <wrtstat/meters/composite_meter.hpp>
#include <wrtstat/meters/time_meter.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <wrtstat/meters/value_meter.hpp>
#include <chrono>

namespace wfc{

using statistics_duration = std::chrono::microseconds;
using composite_point = wrtstat::multi_point< wrtstat::composite_point<statistics_duration> >;
using time_point = wrtstat::multi_point< wrtstat::time_point<statistics_duration> >;
using value_point = wrtstat::multi_point< wrtstat::value_point >;
using size_point = wrtstat::multi_point< wrtstat::size_point > ;

using composite_meter = wrtstat::multi_meter< wrtstat::composite_meter<statistics_duration> >;
using time_meter = wrtstat::multi_meter< wrtstat::time_meter<statistics_duration> >;
using value_meter = wrtstat::multi_meter< wrtstat::value_meter >;
using size_meter = wrtstat::multi_meter< wrtstat::size_meter > ;

}
