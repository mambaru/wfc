#pragma once

#include "server_tcp_config.hpp"
#include "server_config_json.hpp"
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace inet{

struct server_tcp_config_json
{
  FAS_NAME(listen_threads)
  FAS_NAME(worker_threads)
  FAS_NAME(outgoing_warning)
  FAS_NAME(outgoing_limit)

  typedef json::object<
    server_tcp_config,
    fas::type_list_n<
       json::base<server_config_json::type>,
       json::member<n_listen_threads, server_tcp_config, int, &server_tcp_config::listen_threads>,
       json::member<n_worker_threads, server_tcp_config, int, &server_tcp_config::worker_threads>,
       json::member<n_outgoing_warning, server_tcp_config, size_t, &server_tcp_config::outgoing_warning>,
       json::member<n_outgoing_limit, server_tcp_config, size_t, &server_tcp_config::outgoing_limit>
       
    >::type
  > type;
  
  typedef type::serializer serializer; 
};

}}
