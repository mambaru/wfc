#pragma once

#include <wfc/module/domain_options_json.hpp>
#include <wfc/module/domain_config.hpp>
#include <wfc/json.hpp>

namespace wfc{
  
template<typename CustomJson, int Features /*= 0*/, typename StatOptJson /*= nostat_json*/ >
struct domain_config_json_t
{
  typedef StatOptJson stat_json;
  typedef domain_options_json_t<Features, stat_json> domain_options_json;
  typedef CustomJson custom_json;
  typedef typename custom_json::target custom_options;
  typedef typename stat_json::target stat_options;
  typedef domain_config_t<custom_options, stat_options> config_type;

  typedef json::object<
    config_type,
    json::member_list<
      json::base< domain_options_json >,
      json::base< custom_json >
    >,
    json::strict_mode
  > type;

  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};


}
