#pragma once

#include <string>
#include <fas/typemanip/empty_type.hpp>

namespace wfc
{

template<
  typename Req = fas::empty_type,
  typename Res = fas::empty_type,
  typename Opt = fas::empty_type
>
struct tank_config: Opt
{
  typedef Opt user_options;
  typedef Req request_type;
  typedef Res response_type;

  std::string target;
  request_type request;
  response_type response;

  size_t gunfire_size = 1; // количество непрерывных запросов - "очередь выстрелов"
  size_t gunfire_total = 1; // Общее число "выстрелов" 
  time_t gunfire_mks = 1; // Интервал между "очередями"
  size_t gunfire_max_waits = 1; // Макс. число запросов ожидающих ответа, 
  time_t statistics_log_ms = 5000;
  time_t start_delay_ms = 0;

  bool abort_if_bad_gateway = true;
  bool use_api_schema = true;
  bool reg_io = true;
};


struct stat_tank_config
{
  
};

}
