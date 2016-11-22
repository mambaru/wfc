#pragma once

#include <wfc/stat/istat.hpp>
#include <wrtstat/wrtstat_options.hpp>
#include <memory>

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
  virtual meter_ptr create_meter(int id, size_t count) final;
  virtual meter_ptr create_meter(const std::string& name, size_t count) final;
  virtual meter_ptr clone_meter(meter_ptr m, size_t count) final;
  
  int count() const;
  aggregated_ptr pop(int id);
  std::string get_name(int id) const;

private:
  std::shared_ptr<impl> _impl;
};

}
