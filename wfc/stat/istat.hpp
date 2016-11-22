#pragma once

#include <wfc/iinterface.hpp>
#include <wrtstat/time_meter.hpp>
#include <wrtstat/aggregated_data.hpp>
#include <string>

namespace wfc{

struct istat: iinterface
{
  typedef std::chrono::nanoseconds duration_type;
  typedef ::wrtstat::time_meter< duration_type > meter_type;
  typedef std::shared_ptr<meter_type> meter_ptr;
  typedef ::wrtstat::aggregated_data aggregated_type;
  typedef std::unique_ptr<aggregated_type> aggregated_ptr;


  virtual ~istat(){}

  virtual       int reg_name(const std::string& name) = 0; 
  virtual meter_ptr create_meter(int id, size_t count) = 0;
  virtual meter_ptr create_meter(const std::string& name, size_t count) = 0;
  virtual meter_ptr clone_meter(meter_ptr m, size_t count) = 0;
};

}
