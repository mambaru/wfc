#pragma once

#include <wfc/inet/conn/iconnection.hpp>
#include <wfc/memory.hpp>
#include <boost/asio.hpp>
#include <set>
#include <thread>

#include <wfc/inet/context.hpp>
#include <wfc/inet/server_tcp_config.hpp>
#include <wfc/inet/srv/server_tcp_context.hpp>
#include <wfc/inet/srv/ad_worker.hpp>
#include <wfc/inet/srv/ad_acceptor.hpp>
#include <wfc/inet/srv/ad_dgram_acceptor.hpp>
#include <wfc/inet/srv/ad_st_acceptor.hpp>
#include <wfc/inet/srv/ad_mt_acceptor.hpp>
#include <wfc/inet/srv/ad_tcp_socket.hpp>
#include <wfc/inet/srv/ad_server_start.hpp>
#include <wfc/inet/srv/connection_manager.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace inet{

struct aspect_server_tcp: fas::aspect< fas::type_list_n<
  context<server_tcp_context>,
  fas::advice<_configuration_, server_tcp_configurator>,
  fas::advice<_socket_,  ad_tcp_socket>, 
  fas::advice<_worker_,  ad_worker/*< ::boost::asio::ip::tcp::socket >*/ >, 
  fas::advice<_acceptor_, ad_acceptor>,
  fas::advice<_st_acceptor_, ad_st_acceptor>,
  fas::advice<_mt_acceptor_, ad_mt_acceptor>,
  fas::advice<_server_start_, ad_server_start>,
  fas::value< _connection_manager_, std::shared_ptr<connection_manager> >,
  fas::group<_start_, _acceptor_>,
  fas::group<_start_, _server_start_>,
  fas::group<_stop_, _acceptor_>,
  fas::group<_stop_, _worker_>,
  fas::group<_stop_, _server_start_>,
  fas::group<_startup_,  _worker_>,
  fas::group<_startup_,  _server_start_>/*, 
  fas::type< _socket_type_, boost::asio::ip::tcp::socket >*/

>::type>
{
};


struct aspect_server_udp: fas::aspect< fas::type_list_n<
  context<server_tcp_context>,
  fas::advice<_configuration_, server_tcp_configurator>,
  fas::advice<_socket_, ad_udp_socket >, 
  fas::advice<_worker_,  ad_worker/*< ::boost::asio::ip::udp::socket >*/ >, 
  fas::advice<_acceptor_, ad_dgram_acceptor >,
  fas::advice<_server_start_, ad_server_start>,
  fas::value< _connection_manager_, std::shared_ptr<connection_manager> >,
  fas::group<_start_, _acceptor_>,
  fas::group<_start_, _server_start_>,
  fas::group<_stop_, _acceptor_>,
  fas::group<_stop_, _worker_>,
  fas::group<_stop_, _server_start_>,
  fas::group<_startup_,  _worker_>,
  fas::group<_startup_,  _server_start_>/*, 
  fas::type< _socket_type_, boost::asio::ip::udp::socket >*/

>::type>
{
};

}}