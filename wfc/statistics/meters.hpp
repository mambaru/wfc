#pragma once

#include <wrtstat/meters/multi_meter.hpp>
#include <wrtstat/meters/composite_meter.hpp>
#include <wrtstat/meters/time_meter.hpp>
#include <wrtstat/meters/size_meter.hpp>
#include <wrtstat/meters/value_meter.hpp>
#include <chrono>

namespace wfc{

using composite_meter = ::wrtstat::multi_meter< ::wrtstat::composite_meter<std::chrono::microseconds> >;
using time_meter = ::wrtstat::multi_meter< ::wrtstat::time_meter<std::chrono::microseconds> >;
using value_meter = ::wrtstat::multi_meter< ::wrtstat::value_meter >;
using size_meter = ::wrtstat::multi_meter< ::wrtstat::size_meter > ;

typedef std::shared_ptr<composite_meter> composite_meter_ptr;
typedef std::shared_ptr<time_meter> time_meter_ptr;
typedef std::shared_ptr<size_meter> size_meter_ptr;
typedef std::shared_ptr<value_meter> value_meter_ptr;


}
