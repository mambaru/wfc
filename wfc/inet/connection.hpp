#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <wfc/callback/callback_owner.hpp>
#include <wfc/inet/iconnection.hpp>
#include <wfc/inet/tags.hpp>
#include <wfc/inet/adv/ad_splitter.hpp>
#include <wfc/inet/basic/tcp_connection.hpp>
//#include <wfc/inet/basic/aspect_tcp.hpp>

namespace wfc{ namespace inet{

struct aspect_echo: fas::aspect< fas::type_list_n<
  fas::alias<basic::_incoming_, basic::_outgoing_>
  //, basic::aspect_tcp
  
  //fas::advice<_write_, ad_writer>,
  //fas::advice<_start_, ad_start>,
  //fas::advice<_reader_, ad_reader>,
  //fas::alias<_rn_income_, _write_>,
  //fas::alias<_income_, _splitter_>,
  //fas::advice<_splitter_, ad_splitter<> >,
  //fas::stub<_income_>
>::type> {};



template<typename A = fas::aspect<> >
using echo_connection = basic::tcp_connection< typename fas::merge_aspect<A, aspect_echo>::type >;
/*class echo_connection
  : public basic::tcp_connection< typename fas::merge_aspect<A, aspect_echo>::type >
{
  
};*/

}}
