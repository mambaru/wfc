#pragma once 

#include <string>
#include <vector>
#include <iostream>
#include <wfc/jsonrpc/aspect_tcp.hpp>
#include <wfc/jsonrpc/server_tcp_config.hpp>
#include <wfc/inet/server.hpp>
#include <wfc/jsonrpc/method.hpp>
#include <wfc/jsonrpc/connection_stream.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

using ::wfc::inet::context;
  

template<typename ...Args>
struct jsonrpc
  : inet::connection_aspect< typename fas::merge_aspect< fas::aspect<Args...>, aspect_stream<> >::type >
{
  
};

/*
fas::advice< inet::_configuration_, server_tcp_config>,
*/

struct aspect_tcp: fas::merge_aspect<
  fas::aspect<
    fas::advice<inet::_configurator_, server_tcp_configurator>
  >,
  inet::aspect_server_tcp
>::type
{};

struct tcp: inet::server_aspect< aspect_tcp >{};

template<typename ...Args>
struct server_tcp_helper
{
  // TODO: Убрать!!! указать явно!!
  typedef inet::server< Args... > server_base;
};
  
template<typename A = connection_stream<>, typename B = tcp, typename ...Args>
//template<typename A = aspect_stream , typename ...Args>
class server_tcp
  : public server_tcp_helper<A, B, Args...>::server_base
{
public:
  typedef typename server_tcp_helper<A, B, Args...>::server_base super;
  
  /*
  server_tcp(::boost::asio::io_service& io_service)
    : super(io_service)
  {}
  */
  
  server_tcp( ::wfc::io_service& io_service, const server_tcp_config& conf )
    : super(io_service, conf)
  {
    //this->configure(conf);
  }

  server_tcp( std::weak_ptr< wfc::global > g, const server_tcp_config& conf )
    : super(g, conf)
  {
    //this->configure(conf);
  }
  
  /*
  void configure(const server_tcp_config& conf)
  {
    auto cntx = this->server_context();
    cntx.listen_threads = conf.listen_threads;
    cntx.worker_threads = conf.worker_threads;
    cntx.port = conf.port;
    this->server_context(cntx);
    
    auto cntx_conn = this->connection_context();
    cntx_conn.close_after_response = conf.close_after_response;
    this->connection_context(cntx_conn);
  }
  
  void reconfigure(const server_tcp_config& conf)
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
  */

};

}}
