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

statistics::statistics(options_type opt)
  //: _prefixes(opt.prefixes)
{
  opt.step_ts *= 1000;
  _impl = std::make_shared<impl>(opt);
  /*if ( _prefixes.empty() ) 
    _prefixes.push_back("");*/
}

void statistics::enable(bool val)
{
  _impl->enable(val);
}


statistics::meter_ptr statistics::create_meter_prototype(const std::string& time_name) 
{
  return this->create_meter_prototype(time_name, "");
}

statistics::meter_ptr statistics::create_meter_prototype(const std::string& time_name, const std::string& size_name) 
{
  auto now = std::time(0)*1000000;
  auto meter = _impl->create_multi_meter<duration_type>(time_name, size_name, now, 0, 1000);
  meter->reset();
  return meter;
  
  /*
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
      size_id!=-1 ? _impl->create_size_meter(size_id, 0, 1000, 0) : nullptr
    ));
  }
  return meter;
  */
}


statistics::meter_ptr statistics::create_meter(const std::string& time_name)
{
  return this->create_meter(time_name, "", 1);
}

statistics::meter_ptr statistics::create_meter(const std::string& time_name, size_type count)
{
  return this->create_meter(time_name, "", count);
}

statistics::meter_ptr statistics::create_meter(const std::string& time_name, const std::string& size_name, size_type size) 
{
  auto now = std::time(0)*1000000;
  return _impl->create_multi_meter<duration_type>(time_name, size_name, now, size, 1000);

  /*
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
      size_id!=-1 ? _impl->create_size_meter(size_id, now, size, 1000) : nullptr
    ));
  }
  return meter;
  */
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
