#include <wfc/stat/stat.hpp>
#include <wrtstat/wrtstat.hpp>
#include <ctime>

namespace wfc{



class stat::impl
  : public ::wrtstat::wrtstat
{
public:
  impl(const ::wrtstat::wrtstat_options& opt )
    : wrtstat( opt)
  {}
};

stat::stat(const options_type& opt)
  : _prefixes(opt.prefixes)
{
  _impl = std::make_shared<impl>(opt);
  if ( _prefixes.empty() ) 
    _prefixes.push_back("");
}

istat::meter_ptr stat::create_meter(const std::string& rate_name, const std::string& size_name) 
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
    
    meter->push_back( std::make_shared<pair_meter>( 
      rate_id!=-1 ? _impl->create_meter< duration_type >(rate_id, 0, 0) : nullptr,
      rate_id!=-1 ? _impl->create_meter< duration_type >(size_id, 0, 0) : nullptr
    ));
  }
  return meter;
}

istat::meter_ptr stat::clone_meter(meter_ptr m, size_t size )
{
  auto now = std::time(0)*1000000;
  return m->clone(now, size);
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
int stat::count() const
{
  return _impl->count();
}

stat::aggregated_ptr stat::pop(int id)
{
  return _impl->pop(id);
}

std::string stat::get_name(int id) const
{
  return _impl->get_name(id);
}

}
