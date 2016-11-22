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
{
  _impl = std::make_shared<impl>(opt);
}

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
