#pragma once

#include <wrtstat/meters/multi_meter.hpp>
#include <chrono>

namespace wfc{

typedef ::wrtstat::multi_meter<std::chrono::microseconds> multi_meter;

}
