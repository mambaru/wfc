#include <wfc/statistics/statistics.hpp>
#include <wrtstat/wrtstat.hpp>
#include <ctime>
#include <cstdlib>

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
  auto now = std::time(0)*1000000 + std::rand()%1000000;
  auto meter = _impl->create_multi_meter<duration_type>(time_name, size_name, now, 0, 1000);
  meter->reset();
  return meter;
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
  auto now = std::time(0)*1000000 + std::rand()%1000000;
  return _impl->create_multi_meter<duration_type>(time_name, size_name, now, size, 1000);
}



statistics::meter_ptr statistics::create_meter(meter_ptr m, size_type size )
{
  auto now = std::time(0)*1000000 + std::rand()%1000000;
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
