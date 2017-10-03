#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/meters.hpp>
#include <wfc/statistics/stat_options.hpp>
#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/aggregated_data.hpp>
#include <wrtstat/reduced_data.hpp>
#include <memory>

namespace wfc{ namespace statistics{

class statistics
{
  class impl;
public:
  typedef ::wrtstat::aggregated_data aggregated_data;
  typedef ::wrtstat::types::handler_fun_t handler_fun_t;
  typedef std::function< void(const wrtstat::reduced_data&) > aggregator_fun_t;
  typedef std::shared_ptr<aggregated_data> aggregated_ptr;
  typedef std::chrono::microseconds  duration_type; 
  typedef size_meter::size_type size_type;
  typedef value_meter::time_type time_type;
  typedef stat_options options_type;

  virtual ~statistics(){}
  statistics(options_type opt);

  virtual composite_meter_ptr create_composite_prototype(const std::string& time_name, const std::string& read_name, const std::string& write_name);
  virtual time_meter_ptr create_time_prototype(const std::string& time_name);
  virtual size_meter_ptr create_size_prototype(const std::string& size_name);
  virtual value_meter_ptr create_value_prototype(const std::string& value_name);

  virtual composite_meter_ptr create_meter(composite_meter_ptr m, size_type size );
  virtual time_meter_ptr create_meter(time_meter_ptr m, size_type count );
  virtual size_meter_ptr create_meter(size_meter_ptr m, size_type size );
  virtual value_meter_ptr create_meter(value_meter_ptr m, size_type value, size_type count );
  

  //bool add(int id, time_type ts_now, value_type v, size_type count);
  handler_fun_t create_handler( const std::string& name, time_type ts_now);
  aggregator_fun_t create_aggregator( const std::string& name, time_type ts_now);
  int count() const;
  aggregated_ptr pop(int id);
  std::string get_name(int id) const;
  void enable(bool val);
  
  bool del( const std::string& name );
  
private:
  std::shared_ptr<impl> _impl;
  //std::vector<std::string> _prefixes;
};

}}
