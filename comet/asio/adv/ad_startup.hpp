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

namespace mamba{ namespace comet{ namespace inet{

struct ad_startup
{
  template<typename T>
  void operator()(T& t)
  {
    this->do_read(t);
  }
};

}}}
