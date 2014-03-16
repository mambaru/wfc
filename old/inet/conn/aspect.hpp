#pragma once

//#include <wfc/inet/conn/ad_do_active.hpp>
#include <wfc/inet/conn/ad_configure.hpp>
#include <wfc/inet/conn/context.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <wfc/inet/tags.hpp>

namespace wfc{ namespace inet{ namespace conn{

typedef fas::type_list_n<
  fas::value<  _context_,   context >,
  fas::advice< _configure_, ad_configure>,
  fas::group< wfc::inet::_configure_, _configure_>
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  
  // Базовые типы
  fas::type < _socket_type_,  boost::asio::socket_base >,
  fas::value< _socket_ptr_ ,  std::shared_ptr<boost::asio::socket_base> >,
  
  // Заглушки
  fas::stub< _start_ >,
  fas::stub< _stop_ >,
  fas::stub< _dispatch_ >,
  fas::stub< _post_ >,
  
  // связка 
  fas::alias< _incoming_, wfc::inet::_incoming_ >,
  fas::alias< _outgoing_, wfc::inet::_outgoing_ >,
  
  // эхо
  fas::alias< wfc::inet::_outgoing_, wfc::inet::_incoming_ >
>
{};

}}}
