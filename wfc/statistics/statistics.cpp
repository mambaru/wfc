#include <wfc/statistics/statistics.hpp>
#include <wrtstat/wrtstat.hpp>
#include <ctime>

namespace wfc{



class statistics::impl
  : public ::wrtstat::wrtstat
{
public:
  impl(const ::wrtstat::wrtstat_options& opt )
    : wrtstat( opt)
  {}
};

statistics::statistics(const options_type& opt)
  : _prefixes(opt.prefixes)
{
  _impl = std::make_shared<impl>(opt);
  if ( _prefixes.empty() ) 
    _prefixes.push_back("");
}

void statistics::enable(bool val)
{
  _impl->enable(val);
}

statistics::meter_ptr statistics::create_meter_prototype(const std::string& rate_name) 
{
  return this->create_meter_prototype(rate_name);
}

statistics::meter_ptr statistics::create_meter_prototype(const std::string& rate_name, const std::string& size_name) 
{
  auto meter = std::make_shared<meter_type>();
  auto now = std::time(0)*1000000;
  for ( auto prefix : _prefixes )
  {
    int rate_id = -1;
    int size_id = -1;
    if ( !rate_name.empty() )
      rate_id = _impl->reg_name(prefix + rate_name, now );
    if ( !size_name.empty() )
      size_id = _impl->reg_name(prefix + size_name, now );
    
    typedef multi_meter::meter_type pair_meter;
    meter->push_back( std::make_shared<pair_meter>( 
      rate_id!=-1 ? _impl->create_time_meter< duration_type >(rate_id, 0, 0) : nullptr,
      size_id!=-1 ? _impl->create_size_meter(size_id, 0, 0) : nullptr
    ));
  }
  return meter;
}

statistics::meter_ptr statistics::create_meter(const std::string& rate_name)
{
  return this->create_meter(rate_name, "", 0);
}


statistics::meter_ptr statistics::create_meter(const std::string& rate_name, const std::string& size_name, size_type size) 
{
  auto meter = std::make_shared<meter_type>();
  auto now = std::time(0)*1000000;
  for ( auto prefix : _prefixes )
  {
    int rate_id = -1;
    int size_id = -1;
    if ( !rate_name.empty() )
      rate_id = _impl->reg_name(prefix + rate_name, now );
    if ( !size_name.empty() )
      size_id = _impl->reg_name(prefix + size_name, now );
    
    typedef multi_meter::meter_type pair_meter;
    meter->push_back( std::make_shared<pair_meter>( 
      rate_id!=-1 ? _impl->create_time_meter< duration_type >(rate_id, now, size) : nullptr,
      size_id!=-1 ? _impl->create_size_meter(size_id, now, size) : nullptr
    ));
  }
  return meter;
}


statistics::meter_ptr statistics::create_meter(meter_ptr m, size_type size )
{
  auto now = std::time(0)*1000000;
  return m->clone(now, size);
}

statistics::meter_ptr statistics::create_meter(meter_ptr m)
{
  return this->create_meter(m, 1);
}
/*
int stat::reg_name(const std::string& name) 
{
  return _impl->reg_name(name, std::time(0)*1000000 );
}

stat::meter_ptr stat::create_meter(int id, size_t count)
{
  return _impl->create_meter< duration_type >(id, std::time(0)*1000000, count);
}

stat::meter_ptr stat::create_meter(const std::string& name, size_t count)
{
  auto now = std::time(0)*1000000;
  return _impl->create_meter< duration_type >( _impl->reg_name(name, now), now, count);
}

stat::meter_ptr stat::clone_meter(meter_ptr m, size_t count)
{
  auto now = std::time(0)*1000000;
  return m->clone(now, count);
}
*/
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

}
