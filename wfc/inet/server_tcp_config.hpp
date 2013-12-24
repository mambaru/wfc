#pragma once 
#include <wfc/inet/server_config.hpp>


namespace wfc{ namespace inet{

struct server_tcp_config: server_config
{
  int listen_threads;
  int worker_threads;
  
  size_t outgoing_warning;
  size_t outgoing_limit;

  server_tcp_config()
    : server_config()
    , listen_threads(0)
    , worker_threads(0)
    , outgoing_warning(0)
    , outgoing_limit(0)
  {
  }
};


struct server_tcp_configurator: server_configurator
{
  typedef server_tcp_config config_type;

  template<typename T>
  void operator()(T& t, const server_tcp_config& conf)
  {
    server_configurator::operator()(t, conf);
    
    auto cntx = t.server_context();
    //this->configure(t, cntx, conf);
    cntx.listen_threads   = conf.listen_threads;
    cntx.worker_threads   = conf.worker_threads;

    t.server_context(cntx);

    auto cntx_conn = t.connection_context();
    
    cntx_conn.outgoing_warning = conf.outgoing_warning;
    cntx_conn.outgoing_limit = conf.outgoing_limit;
    t.connection_context(cntx_conn);

  }
  
  /*
  template<typename T>
  void configure(T& t, typename T::server_context_type& cntx, const server_tcp_config& conf)
  {
    server_configurator::configure(t, cntx, conf);
    cntx.listen_threads   = conf.listen_threads;
    cntx.worker_threads   = conf.worker_threads;
    cntx.outgoing_warning = conf.outgoing_warning;
    cntx.outgoing_limit   = conf.outgoing_limit;
  }*/
};


}}
