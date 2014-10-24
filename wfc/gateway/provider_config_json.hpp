#pragma once

#include <wfc/gateway/provider_config.hpp>

namespace wfc{ namespace gateway{ 
  
struct provider_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(sequence_mode)
  JSON_NAME(queue_limit)
  JSON_NAME(queue_warning)
  JSON_NAME(timeout_ms)
  
  typedef ::wfc::json::object<
    provider_config,
    fas::type_list_n<
      ::wfc::json::member<n_enabled,       provider_config, bool,   &provider_config::enabled>,
      ::wfc::json::member<n_sequence_mode, provider_config, bool,   &provider_config::sequence_mode>,
      ::wfc::json::member<n_queue_limit,   provider_config, size_t, &provider_config::queue_limit>,
      ::wfc::json::member<n_queue_warning, provider_config, size_t, &provider_config::queue_warning>,
      ::wfc::json::member<n_timeout_ms,    provider_config, time_t, &provider_config::wait_timeout_ms>
    >::type
  > type;
  
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};
      

}}

