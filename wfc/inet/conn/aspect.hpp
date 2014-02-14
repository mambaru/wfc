#pragma once

#include <wfc/inet/conn/ad_do_active.hpp>
#include <wfc/inet/conn/ad_configure.hpp>
#include <wfc/inet/conn/context.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <wfc/inet/tags.hpp>

namespace wfc{ namespace inet{ namespace conn{

typedef fas::type_list_n<
  fas::value<_activity_, std::function<void()> >,
  fas::value<_context_, context >,
  fas::advice<_do_active_, ad_do_active>,
  fas::advice<_configure_, ad_configure>,
  fas::group< wfc::inet::_configure_, _configure_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::type<_socket_type_, boost::asio::socket_base>,
  fas::stub<_close_>,
  fas::stub<_start_> 
>
{};

}}}
