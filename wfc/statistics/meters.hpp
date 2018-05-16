#pragma once

#include <wrtstat/meters/multi_meter.hpp>
#include <wrtstat/meters/composite_meter.hpp>
#include <wrtstat/meters/time_meter.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <wrtstat/meters/value_meter.hpp>
#include <chrono>

namespace wfc{

using statistics_duration = std::chrono::microseconds;
using composite_meter = ::wrtstat::multi_meter< ::wrtstat::composite_meter<statistics_duration> >;
using time_meter = ::wrtstat::multi_meter< ::wrtstat::time_meter<statistics_duration> >;
using value_meter = ::wrtstat::multi_meter< ::wrtstat::value_meter >;
using size_meter = ::wrtstat::multi_meter< ::wrtstat::size_meter > ;

using composite_factory = ::wrtstat::multi_meter_factory< ::wrtstat::composite_meter_factory<statistics_duration> >;
using time_factory = ::wrtstat::multi_meter_factory< ::wrtstat::time_meter_factory<statistics_duration> >;
using value_factory = ::wrtstat::multi_meter_factory< ::wrtstat::value_meter_factory >;
using size_factory = ::wrtstat::multi_meter_factory< ::wrtstat::size_meter_factory > ;

/*
using composite_meter = ::wrtstat::multi_meter< ::wrtstat::composite_meter<statistics_duration> >;
using time_meter = ::wrtstat::multi_meter< ::wrtstat::time_meter<statistics_duration> >;
using value_meter = ::wrtstat::multi_meter< ::wrtstat::value_meter >;
using size_meter = ::wrtstat::multi_meter< ::wrtstat::size_meter > ;

typedef std::shared_ptr<composite_meter> composite_meter_ptr;
typedef std::shared_ptr<time_meter> time_meter_ptr;
typedef std::shared_ptr<size_meter> size_meter_ptr;
typedef std::shared_ptr<value_meter> value_meter_ptr;
*/

}
