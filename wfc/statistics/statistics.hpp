#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/meters.hpp>
#include <wfc/statistics/stat_options.hpp>
#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/aggregated_data.hpp>
#include <memory>

namespace wfc{

class statistics
{
  class impl;
public:
  typedef std::shared_ptr<composite_meter> composite_meter_ptr;
  typedef std::shared_ptr<time_meter> time_meter_ptr;
  typedef std::shared_ptr<size_meter> size_meter_ptr;
  typedef std::shared_ptr<value_meter> value_meter_ptr;
  typedef ::wrtstat::aggregated_data aggregated_data;
  typedef std::shared_ptr<aggregated_data> aggregated_ptr;
  typedef std::chrono::microseconds  duration_type; 
  typedef size_meter::size_type size_type;
  typedef value_meter::time_type time_type;
  typedef stat_options options_type;

  virtual ~statistics(){}
  statistics(options_type opt);

  //virtual meter_ptr create_meter_prototype(const std::string& time_name);
  virtual composite_meter_ptr create_composite_prototype(const std::string& time_name, const std::string& read_name, const std::string& write_name);
  virtual time_meter_ptr create_time_prototype(const std::string& time_name);
  virtual size_meter_ptr create_size_prototype(const std::string& size_name);
  virtual value_meter_ptr create_value_prototype(const std::string& value_name);
  
  
  //virtual composite_ptr create_meter(const std::string& time_name, const std::string& read_name, const std::string& write_name, size_type count, size_type size);
  //virtual meter_ptr create_meter(const std::string& read_name, const std::string& write_name, size_type size);
  //virtual meter_ptr create_meter(const std::string& time_name, size_type count);

  virtual composite_meter_ptr create_meter(composite_meter_ptr m, size_type size );
  virtual time_meter_ptr create_meter(time_meter_ptr m, size_type count );
  virtual size_meter_ptr create_meter(size_meter_ptr m, size_type size );
  virtual value_meter_ptr create_meter(value_meter_ptr m, size_type value, size_type count );
  //virtual meter_ptr create_meter(meter_ptr m, size_type count);

  
  int count() const;
  aggregated_ptr pop(int id);
  std::string get_name(int id) const;
  void enable(bool val);
private:
  std::shared_ptr<impl> _impl;
  //std::vector<std::string> _prefixes;
};

}
