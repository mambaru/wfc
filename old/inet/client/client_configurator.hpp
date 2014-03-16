#pragma once 

#include <wfc/inet/client/client_config.hpp>
#include <string>

namespace wfc{ namespace inet{
  
struct client_configurator
{
  typedef client_config config_type;

  template<typename T>
  void operator()(T& t, const client_config& conf)
  {
    auto cntx = t.client_context();
    cntx.enable_stat = conf.enable_stat;
    cntx.host = conf.host;
    cntx.port = conf.port;
    cntx.worker_threads   = conf.worker_threads;
    t.client_context(cntx);

    auto cntx_conn = t.connection_context();
    cntx_conn.outgoing_warning = conf.outgoing_warning;
    cntx_conn.outgoing_limit = conf.outgoing_limit;
    t.connection_context(cntx_conn);
  }
};

}}