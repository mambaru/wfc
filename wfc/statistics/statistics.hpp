#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/multi_meter.hpp>
#include <wfc/statistics/stat_options.hpp>
#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/aggregated_data.hpp>
#include <memory>

namespace wfc{

class statistics
{
  class impl;
public:
  typedef multi_meter meter_type; 
  typedef std::shared_ptr<meter_type> meter_ptr;
  typedef meter_type::duration_type  duration_type; 
  typedef ::wrtstat::aggregated_data aggregated_data;
  typedef std::shared_ptr<aggregated_data> aggregated_ptr;
  typedef meter_type::size_type size_type;
  typedef meter_type::time_type time_type;
  typedef stat_options options_type;

  virtual ~statistics(){}
  statistics(options_type opt);

  virtual meter_ptr create_meter_prototype(const std::string& time_name);
  virtual meter_ptr create_meter_prototype(const std::string& time_name, const std::string& size_name);
  
  virtual meter_ptr create_meter(const std::string& time_name, const std::string& size_name, size_type size, size_type count);
  virtual meter_ptr create_meter(const std::string& size_name, size_type size, size_type count);
  virtual meter_ptr create_meter(const std::string& time_name, size_type count);

  virtual meter_ptr create_meter(meter_ptr m, size_type size, size_type count );
  virtual meter_ptr create_meter(meter_ptr m, size_type count);

  
  int count() const;
  aggregated_ptr pop(int id);
  std::string get_name(int id) const;
  void enable(bool val);
private:
  std::shared_ptr<impl> _impl;
  //std::vector<std::string> _prefixes;
};

}
