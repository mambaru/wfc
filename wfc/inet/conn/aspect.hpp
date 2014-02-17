#pragma once

//#include <wfc/inet/conn/ad_do_active.hpp>
#include <wfc/inet/conn/ad_configure.hpp>
#include <wfc/inet/conn/context.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <wfc/inet/tags.hpp>

namespace wfc{ namespace inet{ namespace conn{

typedef fas::type_list_n<
  //fas::value<  _activity_,  std::function<void()> >,
  fas::value<  _context_,   context >,
  // В стреам
  /*fas::advice< _do_active_, ad_do_active>, 
  fas::group< _on_read_, _do_active_ >
  */
  fas::advice< _configure_, ad_configure>,
  fas::group< wfc::inet::_configure_, _configure_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::type<_socket_type_, boost::asio::socket_base>,
  fas::value<_socket_ptr_, std::shared_ptr<boost::asio::socket_base> >,
  fas::stub<_start_>,
  fas::stub<_stop_>,
  fas::stub<_dispatch_>,
  fas::stub<_post_>,
  fas::stub<_start_> 
>
{};

}}}
