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

  /*
struct composite_meter
  : public ::wrtstat::multi_meter< ::wrtstat::composite_meter<std::chrono::microseconds> >
{
};

struct time_meter
  : public ::wrtstat::multi_meter< ::wrtstat::time_meter<std::chrono::microseconds> >
{
};

struct value_meter
  : public ::wrtstat::multi_meter< ::wrtstat::value_meter >
{
};

struct size_meter
  : public ::wrtstat::multi_meter< ::wrtstat::size_meter >
{
};
*/

}
