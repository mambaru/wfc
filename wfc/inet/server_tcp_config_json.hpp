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

  typedef json::object<
    server_tcp_config,
    fas::type_list_n<
       json::base<server_config_json::type>,
       json::member<n_listen_threads, server_tcp_config, int, &server_tcp_config::listen_threads>,
       json::member<n_worker_threads, server_tcp_config, int, &server_tcp_config::worker_threads>
    >::type
  > type;
  
  typedef type::serializer serializer; 
};

}}
