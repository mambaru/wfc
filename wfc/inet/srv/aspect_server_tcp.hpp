#pragma once

#include <wfc/inet/conn/iconnection.hpp>
#include <wfc/memory.hpp>
#include <boost/asio.hpp>
#include <set>
#include <thread>

#include <wfc/inet/context.hpp>
#include <wfc/inet/srv/server_tcp_context.hpp>
#include <wfc/inet/srv/ad_worker.hpp>
#include <wfc/inet/srv/ad_acceptor.hpp>
#include <wfc/inet/srv/ad_st_acceptor.hpp>
#include <wfc/inet/srv/ad_mt_acceptor.hpp>
#include <wfc/inet/srv/ad_tcp_socket.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace inet{

struct aspect_server_tcp: fas::aspect< fas::type_list_n<
  context<server_tcp_context>,
  fas::advice<_socket_,  ad_tcp_socket>, 
  fas::advice<_worker_,  ad_worker>, 
  fas::advice<_acceptor_, ad_acceptor>,
  fas::advice<_st_acceptor_, ad_st_acceptor>,
  fas::advice<_mt_acceptor_, ad_mt_acceptor>,
  fas::alias<_start_, _acceptor_>,
  fas::alias<_stop_, _acceptor_>,
  fas::group<_startup_,  _worker_>
>::type>
{
};

}}