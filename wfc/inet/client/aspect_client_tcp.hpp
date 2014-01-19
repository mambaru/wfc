#pragma once 

#include <wfc/inet/client/client_tcp_config.hpp>
#include <wfc/inet/client/client_config.hpp>
#include <wfc/inet/client/client_configurator.hpp>
#include <wfc/inet/client/client_tcp_configurator.hpp>
#include <wfc/inet/client/client_tcp_context.hpp>
#include <wfc/inet/client/ad_connect.hpp>
#include <wfc/inet/client/tags.hpp>
  

namespace wfc{ namespace inet{
  
struct aspect_client_tcp: fas::aspect< fas::type_list_n<
  context<client_tcp_context>,
  fas::advice<_configuration_, client_tcp_configurator>,
  fas::advice<_socket_,  ad_tcp_socket>, 
  fas::advice<_connect_, ad_connect>,
  fas::group<_start_, _connect_>,
  //fas::advice<_server_start_, ad_server_start>,
  fas::value< _connection_manager_, std::shared_ptr<connection_manager<>> >//,
  //fas::group<_start_, _acceptor_>,
  //fas::group<_start_, _server_start_>//,
  //fas::group<_stop_, _acceptor_>,
  //fas::group<_stop_, _worker_>,
  //fas::group<_stop_, _server_start_>,
  //fas::group<_startup_,  _worker_>,
  //fas::group<_startup_,  _server_start_>
  /*, 
  fas::type< _socket_type_, boost::asio::ip::tcp::socket >*/

>::type>
{
};

}}