#pragma once

#include <wfc/memory.hpp>
#include <boost/asio.hpp>
#include <set>
#include <thread>

#include <wfc/inet/context.hpp>
#include <wfc/inet/srv/tmp/server_tcp_config.hpp>
#include <wfc/inet/srv/tmp/server_tcp_context.hpp>
#include <wfc/inet/srv/tmp/ad_worker.hpp>
#include <wfc/inet/srv/tmp/ad_acceptor.hpp>
#include <wfc/inet/srv/tmp/ad_dgram_acceptor.hpp>
#include <wfc/inet/srv/tmp/ad_st_acceptor.hpp>
#include <wfc/inet/srv/tmp/ad_mt_acceptor.hpp>
#include <wfc/inet/srv/tmp/ad_tcp_socket.hpp>
#include <wfc/inet/srv/tmp/ad_server_start.hpp>
#include <wfc/inet/srv/tmp/connection_manager.hpp>
#include <wfc/inet/srv/tags.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace inet{

struct aspect_server_tcp: fas::aspect< fas::type_list_n<
  context<server_tcp_context>,
  fas::advice<srv::_configurator_, server_tcp_configurator>,
  fas::advice<_socket_,  ad_tcp_socket>, 
  // Временная мера
  fas::type<srv::_acceptor_type_, ::boost::asio::ip::tcp::acceptor>, 
  fas::advice<_worker_,  ad_worker/*< ::boost::asio::ip::tcp::socket >*/ >, 
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
  fas::group<_startup_,  _server_start_>/*, 
  fas::type< _socket_type_, boost::asio::ip::tcp::socket >*/

>::type>
{
};


struct aspect_server_udp: fas::aspect< fas::type_list_n<
  context<server_tcp_context>,
  fas::advice<srv::_configurator_, server_tcp_configurator>,
  fas::advice<_socket_, ad_udp_socket >, 
  fas::advice<_worker_,  ad_worker/*< ::boost::asio::ip::udp::socket >*/ >, 
  fas::advice<_acceptor_, ad_dgram_acceptor >,
  fas::advice<_server_start_, ad_server_start>,
  connection_manager_type<connection_manager>, 
  // Временная мера
  fas::type<srv::_acceptor_type_, ::boost::asio::ip::tcp::acceptor>, 
  //fas::value< _connection_manager_, std::shared_ptr<connection_manager<>> >,
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