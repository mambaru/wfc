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
  impl(const ::wrtstat::wrtstat_options& opt )
    : wrtstat( opt)
  {}
};

statistics::~statistics()
{
  _impl = nullptr;
}
statistics::statistics(options_type opt)
{
  _impl = std::make_shared<impl>(opt);
}

void statistics::enable(bool val)
{
  _impl->enable(val);
}

/*
size_t statistics::count() const
{
  return _impl->aggregators_count();
}
*/

value_factory statistics::create_value_factory(const std::string& name)
{
  return _impl->create_value_multi_meter_factory(name);
}

size_factory statistics::create_size_factory(const std::string& name)
{
  return _impl->create_size_multi_meter_factory(name);
}

time_factory statistics::create_time_factory(const std::string& name)
{
  return _impl->create_time_multi_meter_factory<statistics_duration>(name);
}


composite_factory statistics::create_composite_factory(const std::string& time, const std::string& read, const std::string& write, bool summary_size)
{
  return _impl->create_composite_multi_meter_factory<statistics_duration>(time, read, write, summary_size);
}
  /*
class statistics::impl
  : public ::wrtstat::wrtstat_mt
{
public:
  impl(const ::wrtstat::wrtstat_options& opt )
    : wrtstat_mt( opt)
  {}
};

statistics::statistics(options_type opt)
{
  _impl = std::make_shared<impl>(opt);
  _resolution = opt.resolution;
}

bool statistics::add( const std::string& name, const reduced_data& v)
{
  return _impl->add(name, v);
}

bool statistics::del( const std::string& name )
{
  return _impl->del(name);
}

void statistics::enable(bool val)
{
  _impl->enable(val);
}

composite_meter_ptr 
  statistics::create_composite_prototype(const std::string& time_name, const std::string& read_name, const std::string& write_name)
{
  if (time_name.empty() && read_name.empty() && write_name.empty() ) 
    return nullptr;
  auto now = ::wrtstat::aggregator::now(_resolution);
  auto meter = _impl->create_multi_meter<duration_type>(time_name, read_name, write_name, now, 0);
  meter->reset();
  return meter;
}

time_meter_ptr 
  statistics::create_time_prototype(const std::string& time_name)
{
  if (time_name.empty() ) 
    return nullptr;

  auto now = ::wrtstat::aggregator::now(_resolution);
  auto meter = _impl->create_multi_meter< ::wrtstat::time_meter<duration_type> >(time_name, now, 0);
  meter->reset();
  return meter;
}

size_meter_ptr 
  statistics::create_size_prototype(const std::string& size_name)
{
  if (size_name.empty() ) 
    return nullptr;

  auto now = ::wrtstat::aggregator::now(_resolution);
  auto meter = _impl->create_multi_meter< ::wrtstat::size_meter >(size_name, now, 0);
  meter->reset();
  return meter;

}

value_meter_ptr 
  statistics::create_value_prototype(const std::string& value_name)
{
  if (value_name.empty() ) 
    return nullptr;

  auto now = ::wrtstat::aggregator::now(_resolution);
  auto meter = _impl->create_multi_meter< ::wrtstat::value_meter >(value_name, now, 0);
  meter->reset();
  return meter;
}


composite_meter_ptr 
  statistics::create_meter(composite_meter_ptr m, size_type size )
{
  if ( m == nullptr ) return nullptr;
  auto now = ::wrtstat::aggregator::now(_resolution);
  return m->clone(now, size);
}

time_meter_ptr 
  statistics::create_meter(time_meter_ptr m, size_type count )
{
  if ( m == nullptr ) return nullptr;
  auto now = ::wrtstat::aggregator::now(_resolution);
  return m->clone(now, count);
}

size_meter_ptr 
  statistics::create_meter(size_meter_ptr m, size_type size )
{
  if ( m == nullptr ) return nullptr;
  auto now = ::wrtstat::aggregator::now(_resolution);
  return m->clone(now, size);
}

value_meter_ptr 
  statistics::create_meter(value_meter_ptr m, size_type value, size_type count )
{
  if ( m == nullptr ) return nullptr;
  auto now = ::wrtstat::aggregator::now(_resolution);
  return m->clone(now, value, count);
}

statistics::value_adder_t statistics::create_value_adder(const std::string& name, time_type ts_now)
{
  return _impl->create_value_adder( name, ts_now );
}


statistics::data_adder_t statistics::create_data_adder(const std::string& name, time_type ts_now)
{
  return _impl->create_data_adder( name, ts_now );
}

statistics::reduced_adder_t statistics::create_reduced_adder( const std::string& name, time_type ts_now)
{
  return _impl->create_reduced_adder( name, ts_now );
}

int statistics::count() const
{
  return _impl->count();
}

statistics::aggregated_ptr statistics::pop(int id)
{
  return _impl->pop(id);
}

std::string statistics::get_name(int id) const
{
  return _impl->get_name(id);
}
*/

}}
