//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wfc/statistics/statistics.hpp>
#include <wfc/statistics/meters.hpp>
#include <wrtstat/wrtstat.hpp>
#include <ctime>
#include <cstdlib>

namespace wfc{ namespace statistics{

class statistics::impl
  : public ::wrtstat::wrtstat
{
public:
  explicit impl(const ::wrtstat::wrtstat_options& opt )
    : wrtstat( opt)
  {}
};

statistics::~statistics()
{
  _impl = nullptr;
}

statistics::statistics(const options_type& opt)
  : _prefixes(opt.prefixes)
{
  _impl = std::make_shared<impl>(opt);
}

void statistics::enable(bool val)
{
  _impl->enable(val);
}

bool statistics::add(const std::string& name, const wrtstat::reduced_data& v)
{
  return _impl->add(name, v);
}

bool statistics::del(const std::string& name)
{
  return _impl->del(name);
}

size_t statistics::aggregators_count() const
{
  return _impl->aggregators_count();
}

std::string statistics::get_name(size_t i) const
{
  return _impl->get_name(i);
}

statistics::aggregated_ptr statistics::pop(size_t i)
{
  return _impl->pop(i);
}

value_meter statistics::create_value_meter(const std::string& name)
{
  return _impl->create_value_multi_meter(_prefixes, name);
}

size_meter statistics::create_size_meter(const std::string& name)
{
  return _impl->create_size_multi_meter(_prefixes, name);
}

time_meter statistics::create_time_meter(const std::string& name)
{
  return _impl->create_time_multi_meter<statistics_duration>(_prefixes, name);
}


composite_meter statistics::create_composite_meter(const std::string& time, const std::string& read, const std::string& write, bool summary_size)
{
  return _impl->create_composite_multi_meter<statistics_duration>(_prefixes, time, read, write, summary_size);
}


}}
