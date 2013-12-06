#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <comet/callback/callback_owner.hpp>
#include <comet/asio/iconnection.hpp>
#include <comet/asio/tags.hpp>


namespace mamba{ namespace comet{ namespace inet{

template<typename Tg, typename N>
struct ad_trimmer
{
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    if ( d->size() >= N::value )
      d->resize( d->size() - N::value );
    else
      d->clear();
    t.get_aspect().template get<Tg>()(t, std::move(d));
  }
};

}}}
