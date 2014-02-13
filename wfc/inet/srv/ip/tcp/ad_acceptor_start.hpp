#pragma once 

#include <efc/inet/srv/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{
 
// TODO: перенести в inet::srv
struct ad_acceptor_start
{
  template<typename T>
  void operator()(T& t, fas::tag<_start_>)
  {
    int threads = t.server_context().listen_threads;
  }
};

  
}}}}}
