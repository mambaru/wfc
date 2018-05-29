#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/stat_options.hpp>
#include <wfc/statistics/meters.hpp>
#include <wrtstat/wrtstat.hpp>
#include <memory>

namespace wfc{ namespace statistics{

class statistics
{
  class impl;
public:
  typedef wrtstat::wrtstat_options options_type;
  typedef wrtstat::aggregated_data::ptr aggregated_ptr;

  virtual ~statistics();
  statistics(options_type opt);
  void enable(bool val);
  size_t aggregators_count() const;
  std::string get_name(size_t i) const;
  aggregated_ptr pop(size_t i);
  
  value_factory create_value_factory(const std::string& name);
  size_factory create_size_factory(const std::string& name);
  time_factory create_time_factory(const std::string& name);
  composite_factory create_composite_factory(const std::string& time, const 
    std::string& read, const std::string& write, bool summary_size);
private:
  std::shared_ptr<impl> _impl;
};


/*typedef ::wrtstat::wrtstat statistics;*/
  /*
class statistics
{
  class impl;
public:
  typedef ::wrtstat::aggregated_data aggregated_data;
  typedef ::wrtstat::reduced_data reduced_data;
  typedef ::wrtstat::time_type time_type;
  typedef ::wrtstat::size_type size_type;
  typedef ::wrtstat::data_type data_type;
  typedef std::function< void(time_type now, time_type value, size_type count) > value_adder_t;
  typedef std::function< void(time_type now, data_type&& value, size_type count) > data_adder_t;
  typedef std::function< void(const reduced_data&) > reduced_adder_t;

  typedef std::shared_ptr<aggregated_data> aggregated_ptr;
  typedef std::chrono::microseconds  duration_type; 
    typedef stat_options options_type;

  virtual ~statistics(){}
  statistics(options_type opt);
  
  bool add( const std::string& name, const reduced_data& v);
  bool del( const std::string& name );
  
  composite_meter_ptr create_composite_prototype(const std::string& time_name, const std::string& read_name, const std::string& write_name);
  time_meter_ptr create_time_prototype(const std::string& time_name);
  size_meter_ptr create_size_prototype(const std::string& size_name);
  value_meter_ptr create_value_prototype(const std::string& value_name);

  composite_meter_ptr create_meter(composite_meter_ptr m, size_type size );
  time_meter_ptr create_meter(time_meter_ptr m, size_type count );
  size_meter_ptr create_meter(size_meter_ptr m, size_type size );
  value_meter_ptr create_meter(value_meter_ptr m, size_type value, size_type count );
  
  value_adder_t create_value_adder( const std::string& name, time_type ts_now);
  data_adder_t create_data_adder( const std::string& name, time_type ts_now);
  reduced_adder_t create_reduced_adder( const std::string& name, time_type ts_now);
  int count() const;
  aggregated_ptr pop(int id);
  std::string get_name(int id) const;
  void enable(bool val);
private:
  std::shared_ptr<impl> _impl;
  time_type _resolution;
};
*/

}}
