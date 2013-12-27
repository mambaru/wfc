#pragma once 

#include <wfc/inet/server.hpp>


namespace wfc{ namespace jsonrpc{
  
template<typename ...Args>
class server_tcp
  : public server<Args...>
{
public:
  typedef server<Args...> super;
  
  server( ::wfc::io_service& io_service, const server_config& conf )
    : super(io_service, conf)
  {
    this->configure(conf);
  }

  
  server_tcp( std::weak_ptr< wfc::global > g, const server_tcp_config& conf )
    : super(g, conf)
  {
    this->configure(conf);
  }
  
  void configure(const server_tcp_config& conf)
  {
    std::cout << "server_tcp configure" << std::endl;
    auto cntx = this->server_context();
    cntx.listen_threads = conf.listen_threads;
    cntx.worker_threads = conf.worker_threads;
    cntx.port = conf.port;
    this->server_context(cntx);

    auto cntx_conn = this->server_context();
    cntx.outgoing_warning = conf.outgoing_warning;
    cntx.outgoing_limit = conf.outgoing_limit;
    this->connection_context(cntx_conn);
  }
  
  void reconfigure(const server_config& conf)
  {
    this->configure(conf);
    super::reconfigure(conf);
  }
  
  void initialize()
  {
    super::initialize();
  }
  
  void start()
  {
    super::start();
  }
  
  void stop()
  {
    super::stop();
  }

};

}}
