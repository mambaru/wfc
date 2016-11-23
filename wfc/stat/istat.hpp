#pragma once

#include <wfc/iinterface.hpp>
#include <wrtstat/time_meter.hpp>
#include <wrtstat/aggregated_data.hpp>
#include <string>

namespace wfc{

class pair_meter
{
public:
  typedef std::shared_ptr<pair_meter> self_ptr;
  typedef std::chrono::nanoseconds duration_type;
  typedef ::wrtstat::time_meter< duration_type > meter_type;
  typedef meter_type::time_type  time_type;
  typedef meter_type::size_type size_type;
  typedef std::shared_ptr<meter_type> meter_ptr;

  pair_meter(meter_ptr rm, meter_ptr sm)
    : _rate_meter(rm)
    , _size_meter(sm)
  {
  }

  self_ptr clone(time_type now, size_t size) const
  {
    meter_ptr rate_meter;
    meter_ptr size_meter;
    if ( _rate_meter!=nullptr )
      rate_meter = _rate_meter->clone(now, 1);
    if ( _size_meter!=nullptr )
      size_meter = _size_meter->clone(now, size);

    return std::make_shared<pair_meter>(rate_meter, size_meter);
  }

private:
  meter_ptr _rate_meter;
  meter_ptr _size_meter;
};

class multi_meter
{
public:
  typedef std::shared_ptr<multi_meter> self_ptr;
  typedef pair_meter meter_type;
  typedef std::shared_ptr<meter_type> meter_ptr;
  typedef std::vector<meter_ptr> meter_list;
  typedef meter_type::time_type time_type;
  typedef meter_type::size_type size_type;

  void push_back(meter_ptr p)
  {
    _meters.push_back(p);
  }
  
  self_ptr clone(time_type now, size_type size) const
  {
    auto m = std::make_shared<multi_meter>();
    m->_meters.reserve(this->_meters.size());
    for (auto p : _meters )
    {
      m->_meters.push_back( p->clone(now, size) );
    }
    return  m;
  }

private:
  meter_list _meters;
};


struct istat: iinterface
{
  typedef std::chrono::nanoseconds duration_type;
  typedef multi_meter meter_type;
  typedef std::shared_ptr<meter_type> meter_ptr;
  typedef ::wrtstat::aggregated_data aggregated_type;
  typedef std::unique_ptr<aggregated_type> aggregated_ptr;

  virtual ~istat(){}

  virtual meter_ptr create_meter(const std::string& rate_name, const std::string& size_name) = 0;
  virtual meter_ptr clone_meter(meter_ptr m, size_t count ) = 0;
};

}
