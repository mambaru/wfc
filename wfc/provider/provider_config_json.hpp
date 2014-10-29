#pragma once

#include <wfc/provider/provider_config.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace provider{ 

namespace wj = ::wfc::json;

struct provider_config_json
{
  JSON_NAME2(n_auto, "auto")
  JSON_NAME(simple)
  JSON_NAME(connectify)
  JSON_NAME(insured)
  JSON_NAME(sequenced)

  typedef fas::type_list_n<
    wj::enum_value<n_auto, provider_mode, provider_mode::_auto_>,
    wj::enum_value<n_simple, provider_mode, provider_mode::simple>,
    wj::enum_value<n_connectify, provider_mode, provider_mode::connectify>,
    wj::enum_value<n_insured, provider_mode, provider_mode::insured>,
    wj::enum_value<n_sequenced, provider_mode, provider_mode::sequenced>
  >::type mode_enum_list;

 typedef wj::enumerator< provider_mode, mode_enum_list> mode_enum_json;

  JSON_NAME(enabled)
  JSON_NAME(mode)
  JSON_NAME(max_waiting)
  JSON_NAME(queue_limit)
  JSON_NAME(wait_timeout_ms)
  JSON_NAME(queue_warning)
  
  typedef ::wfc::json::object<
    provider_config,
    fas::type_list_n<
      wj::member<n_enabled,       provider_config, bool,          &provider_config::enabled>,
      wj::member<n_mode,          provider_config, provider_mode, &provider_config::mode, mode_enum_json>,
      wj::member<n_max_waiting,   provider_config, size_t,        &provider_config::max_waiting>,
      wj::member<n_queue_limit,   provider_config, size_t,        &provider_config::queue_limit>,
      wj::member<n_wait_timeout_ms,    provider_config, time_t,   &provider_config::wait_timeout_ms>, 
      wj::member<n_queue_warning, provider_config, size_t,        &provider_config::queue_warning>
    >::type
  > type;

  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};
      

}}

