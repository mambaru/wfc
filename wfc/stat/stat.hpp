#pragma once

#include <wfc/stat/istat.hpp>
#include <wrtstat/wrtstat_options.hpp>

namespace wfc{

class stat: public istat
{
  class impl;
public:
  typedef istat::meter_type meter_type; 
  typedef istat::meter_ptr  meter_ptr;
  typedef istat::duration_type  duration_type; 
  typedef ::wrtstat::wrtstat_options options_type;
  virtual ~stat(){}
  stat(const options_type& opt);
  virtual       int reg_name(const std::string& name) final;
  virtual meter_ptr create_handler(int id) final;
  virtual meter_ptr create_handler(const std::string& name) final;
private:
  std::shared_ptr<impl> _impl;
};

}
