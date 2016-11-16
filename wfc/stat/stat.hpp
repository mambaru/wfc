#pragma once

#include <wfc/stat/istat.hpp>

namespace wfc{

class stat: public istat
{
  class impl;
public:
  typedef istat::time_meter time_meter; 
  typedef istat::meter_ptr  meter_ptr; 
  virtual ~stat(){}
  virtual       int reg_name(const std::string& name) final;
  virtual meter_ptr create_handler(int id) final;
  virtual meter_ptr create_handler(const std::string& name) final;

};

}
