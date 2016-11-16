#pragma once

#include <wfc/iinterface.hpp>
#include <wrtstat/time_meter.hpp>
#include <string>

namespace wfc{

struct istat: iinterface
{
  struct time_meter: ::wrtstat::time_meter< std::chrono::nanoseconds > {};
  typedef std::shared_ptr<time_meter> meter_ptr;

  virtual ~istat(){}

  virtual       int reg_name(const std::string& name) = 0; 
  virtual meter_ptr create_handler(int id) = 0;
  virtual meter_ptr create_handler(const std::string& name) = 0;
};

}
