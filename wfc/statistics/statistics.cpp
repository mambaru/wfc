#include <wfc/statistics/statistics.hpp>
#include <wrtstat/wrtstat.hpp>
#include <ctime>
#include <cstdlib>

namespace wfc{ namespace statistics{

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
  opt.step_ts *= 1000;
  _impl = std::make_shared<impl>(opt);
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
  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  auto meter = _impl->create_multi_meter<duration_type>(time_name, read_name, write_name, now, 0);
  meter->reset();
  return meter;
}

time_meter_ptr 
  statistics::create_time_prototype(const std::string& time_name)
{
  if (time_name.empty() ) 
    return nullptr;

  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  auto meter = _impl->create_multi_meter< ::wrtstat::time_meter<duration_type> >(time_name, now, 0);
  meter->reset();
  return meter;
}

size_meter_ptr 
  statistics::create_size_prototype(const std::string& size_name)
{
  if (size_name.empty() ) 
    return nullptr;

  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  auto meter = _impl->create_multi_meter< ::wrtstat::size_meter >(size_name, now, 0);
  meter->reset();
  return meter;

}

value_meter_ptr 
  statistics::create_value_prototype(const std::string& value_name)
{
  if (value_name.empty() ) 
    return nullptr;

  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  auto meter = _impl->create_multi_meter< ::wrtstat::value_meter >(value_name, now, 0);
  meter->reset();
  return meter;
}


composite_meter_ptr 
  statistics::create_meter(composite_meter_ptr m, size_type size )
{
  if ( m == nullptr ) return nullptr;
  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  return m->clone(now, size);
}

time_meter_ptr 
  statistics::create_meter(time_meter_ptr m, size_type count )
{
  if ( m == nullptr ) return nullptr;
  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  return m->clone(now, count);
}

size_meter_ptr 
  statistics::create_meter(size_meter_ptr m, size_type size )
{
  if ( m == nullptr ) return nullptr;
  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  return m->clone(now, size);
}

value_meter_ptr 
  statistics::create_meter(value_meter_ptr m, size_type value, size_type count )
{
  if ( m == nullptr ) return nullptr;
  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  return m->clone(now, value, count);
}

/*

statistics::meter_ptr statistics::create_meter_prototype(const std::string& time_name) 
{
  return this->create_meter_prototype(time_name, "", "");
}

statistics::meter_ptr statistics::create_meter_prototype(
  const std::string& time_name, 
  const std::string& read_name,
  const std::string& write_name) 
{
  //auto now = std::time(0)*1000000 + std::rand()%1000000;
  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  auto meter = _impl->create_multi_meter<duration_type>(time_name, read_name, write_name, now, 0, 0);
  meter->reset();
  return meter;
}


statistics::meter_ptr statistics::create_meter(const std::string& time_name, size_type count)
{
  return this->create_meter(time_name, "", "", count, 0);
}

statistics::meter_ptr statistics::create_meter(const std::string& read_name, const std::string& write_name, size_type size)
{
  return this->create_meter("", read_name, "", 0, size);
}

statistics::meter_ptr statistics::create_meter(
    const std::string& time_name, 
    const std::string& read_name, 
    const std::string& write_name, 
    size_type count, size_type size) 
{
  //auto now = std::time(0)*1000000 + std::rand()%1000000;
  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  return _impl->create_multi_meter<duration_type>(time_name, read_name, write_name, now, count, size);
}



statistics::meter_ptr statistics::create_meter(meter_ptr m, size_type count, size_type size )
{
  if ( m == nullptr )
    return nullptr;
  auto now = ::wrtstat::aggregator::now<std::chrono::microseconds>();
  return m->clone(now, count, size);
}


statistics::meter_ptr statistics::create_meter(meter_ptr m, size_type count)
{
  if ( m == nullptr )
    return nullptr;
  return this->create_meter(m, count, 0);
}
*/

statistics::handler_fun_t statistics::create_handler(const std::string& name, time_type ts_now)
{
  return _impl->create_handler( name, ts_now );
}

statistics::aggregator_fun_t statistics::create_aggregator( const std::string& name, time_type ts_now)
{
  return _impl->create_aggregator_handler( name, ts_now );
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


}}
