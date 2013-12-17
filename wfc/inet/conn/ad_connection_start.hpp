#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <wfc/callback/callback_owner.hpp>
#include <wfc/inet/iconnection.hpp>
#include <wfc/inet/tags.hpp>


namespace wfc{ namespace inet{

template<typename Tg>
struct ad_connection_start
{
  template<typename T>
  void operator()(T& t, fas::tag<_start_>)
  {
    t.get_aspect().template get<Tg>()(t);
  }
};

}}
