#pragma once 

#include <string>
#include <vector>
#include <iostream>
#include <wfc/jsonrpc/aspect.hpp>
#include <wfc/inet/server.hpp>
#include <wfc/inet/jsonrpc/method.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Tg, typename M>
using method = ::wfc::inet::jsonrpc::method<Tg, M>;
  
template<typename ...Args>
struct jsonrpc
  : inet::connection_aspect< typename fas::merge_aspect< fas::aspect<Args...>, aspect>::type >
{
  
};


template<typename ...Args>
struct server_tcp_helper
{
  typedef inet::server< fas::aspect<
    Args...
  > > server_base;
};
  
template<typename A = jsonrpc<> , typename ...Args>
class server_tcp
  : public server_tcp_helper<A, Args...>::server_base
{
public:
  typedef typename server_tcp_helper<A, Args...>::server_base super;
  
  server_tcp(::boost::asio::io_service& io_service)
    : super(io_service)
  {}

};

}}
