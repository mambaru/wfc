#pragma once

#include <wfc/inet/conn/aspect.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace stream{

typedef fas::type_list_n<
  wfc::inet::conn::advice_list
>::type advice_list;

struct aspect: fas::aspect
< 
  advice_list
>
{};


/*

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

*/

// Перенести часть в connection
/*
struct aspect: fas::aspect
< 
  ::wfc::inet::context<  >,
  basic_context_class<basic_context>, 
  fas::advice<_start_,    ad_connection_start<_reader_> >,
  fas::advice<_do_active_, ad_do_active>,
  fas::group<_on_read_,   _do_active_ >,
  //fas::alias<_on_read_,   _incoming_ >,
  fas::advice<_outgoing_, ad_outgoing >,
  fas::alias<_write_,     _writer_ >,  
  fas::advice<_writer_,   ad_writer<> >,
  fas::advice<_on_write_, ad_on_write>, 
  fas::advice<_shutdown_, ad_shutdown>,
  fas::stub<_write_error_>, 
  fas::stub<_read_error_>, 
  fas::stub<_alive_error_>,
  fas::value<_activity_, std::function<void()> >
  
>
{};
*/

}}}}
