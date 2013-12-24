#pragma once 

#include <wfc/inet/server_tcp_config.hpp>

namespace wfc{ namespace jsonrpc{

struct server_tcp_config
  : inet::server_tcp_config
{
  bool close_after_response;
  server_tcp_config()
    : close_after_response(false)
  {}
};


struct server_tcp_configurator: inet::server_tcp_configurator
{
  typedef server_tcp_config config_type;

  template<typename T>
  void operator()(T& t, const server_tcp_config& conf)
  {
    std::cout << "############################ reconfig jsonrpc" << std::endl;
    inet::server_tcp_configurator::operator()(t, conf);
   
    auto cntx_conn = t.connection_context();
    cntx_conn.close_after_response = conf.close_after_response;
    t.connection_context(cntx_conn);

    /*
    auto cntx = t.server_context();
    //this->configure(t, cntx, conf);
    cntx.listen_threads   = conf.listen_threads;
    cntx.worker_threads   = conf.worker_threads;

    t.server_context(cntx);

    auto cntx_conn = t.connection_context();
    
    cntx_conn.outgoing_warning = conf.outgoing_warning;
    cntx_conn.outgoing_limit = conf.outgoing_limit;
    t.connection_context(cntx_conn);
    */

  }
};



}}
