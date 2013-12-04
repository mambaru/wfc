#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <comet/callback/callback_owner.hpp>
#include <comet/asio/iconnection.hpp>
#include <comet/asio/tags.hpp>
#include <comet/asio/adv/ad_writer.hpp>
#include <comet/asio/adv/ad_reader.hpp>
#include <comet/asio/adv/ad_splitter.hpp>
#include <comet/asio/basic/tcp_connection.hpp>
//#include <comet/asio/basic/aspect_tcp.hpp>

namespace mamba{ namespace comet{ namespace inet{

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
class echo_connection
  : public basic::tcp_connection< typename fas::merge_aspect<A, aspect_echo>::type >
{
  
};

}}}
