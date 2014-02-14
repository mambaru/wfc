#pragma once 

#include <fas/aop.hpp>
#include <wfc/inet/srv/ip/tcp/types.hpp>
#include <wfc/inet/srv/ip/tcp/basic_context.hpp>
#include <wfc/inet/srv/ip/tcp/configurator.hpp>
#include <wfc/inet/srv/ip/tcp/ad_acceptor_open.hpp>
#include <wfc/inet/srv/ip/tcp/ad_acceptor_start.hpp>
#include <wfc/inet/srv/ip/tcp/ad_new_connection.hpp>
#include <wfc/inet/srv/ip/tcp/ad_socket_control.hpp>
#include <wfc/inet/srv/tags.hpp>

#include <wfc/inet/context.hpp>
#include <wfc/inet/basic_context_class.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{
  
struct aspect: fas::aspect
<
  wfc::inet::context<>,
  wfc::inet::basic_context_class< wfc::inet::srv::ip::tcp::basic_context >,
  fas::type   < _acceptor_type_,  acceptor_type >,
  fas::advice < _acceptor_open_,  ad_acceptor_open >,
  fas::advice < _acceptor_start_, ad_acceptor_start >,
  fas::advice < _new_connection_, ad_new_connection >,
  fas::advice < _configurator_,   configurator >,
  fas::advice < _socket_control_, ad_socket_control>,
  fas::value< _acceptor_,         acceptor_ptr >,
  connection_manager_type<connection_manager>, 
  fas::group<_startup_, _acceptor_open_>,
  fas::group<_start_, _acceptor_start_>
  
  /*
  context<server_tcp_context>,
  fas::advice<_configurator_, server_tcp_configurator>,
  fas::advice<_socket_,  ad_tcp_socket>, 
  fas::type<_acceptor_type_, ::boost::asio::ip::tcp::acceptor>, 
  fas::advice<_worker_,  ad_worker >, 
  fas::advice<_acceptor_, ad_acceptor>,
  fas::advice<_st_acceptor_, ad_st_acceptor>,
  fas::advice<_mt_acceptor_, ad_mt_acceptor>,
  fas::advice<_server_start_, ad_server_start>,
  connection_manager_type<connection_manager>, 
 // fas::value< _connection_manager_, std::shared_ptr<connection_manager<>> >,
  fas::group<_start_, _acceptor_>,
  fas::group<_start_, _server_start_>,
  fas::group<_stop_, _server_start_>,
  fas::group<_stop_, _acceptor_>,
  fas::group<_stop_, _worker_>,
  fas::group<_startup_,  _worker_>,
  fas::group<_startup_,  _server_start_>, 
  */
>
{};
  
}}}}}
