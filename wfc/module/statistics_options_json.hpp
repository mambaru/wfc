//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/statistics_options.hpp>
#include <wfc/json.hpp>

namespace wfc{
  
template<typename T>
struct empty_stat_json_t
{
  typedef json::object<
    T,
    json::member_list<
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

struct nostat_json: empty_stat_json_t<nostat> {};

struct defstat_json: empty_stat_json_t<defstat> {};

template<typename StatOptJson = nostat_json >
struct statistics_options_json_t;

template<>
struct statistics_options_json_t<nostat_json>
{
  typedef json::object<
    statistics_options_t<nostat>,
    json::member_list<
      json::base<nostat_json>
    >
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

template<typename StatOptJson>
struct statistics_options_json_t
{
  typedef typename StatOptJson::target base_options;
  typedef statistics_options_t<base_options> statistics_type;
  JSON_NAME(disabled)
  JSON_NAME(target)

  typedef json::object<
    statistics_type,
    json::member_list<
      json::member<n_disabled, statistics_type, bool, &statistics_type::disabled>,
      json::member<n_target, statistics_type, std::string, &statistics_type::target>,
      json::base<StatOptJson>
    >, 
    json::strict_mode
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};


}
