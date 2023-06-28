//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/statistics_options.hpp>
#include <wfc/statistics/meters.hpp>
#include <wrtstat/wrtstat.hpp>
#include <memory>

namespace wfc{ namespace statistics{

class statistics
{
  class impl;
public:
  typedef statistics_options options_type;
  typedef wrtstat::aggregated_data::ptr aggregated_ptr;

  virtual ~statistics();
  explicit statistics(const options_type& opt);
  void enable(bool val);
  size_t aggregators_count() const;
  std::string get_name(size_t i) const;
  aggregated_ptr pop(size_t i);
  
  bool add(const std::string& name, const wrtstat::reduced_data& v);
  bool del(const std::string& name);

  value_meter create_value_meter(const std::string& name);
  size_meter create_size_meter(const std::string& name);
  time_meter create_time_meter(const std::string& name);
  composite_meter create_composite_meter(const std::string& time, const 
    std::string& read, const std::string& write, bool summary_size);
private:
  std::vector<std::string> _prefixes;
  std::shared_ptr<impl> _impl;
};

}}
