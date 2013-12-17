#pragma once

#include <wfc/inet/echo_connection.hpp>
#include <wfc/inet/iconnection.hpp>
#include <wfc/inet/server_config.hpp>
#include <wfc/memory.hpp>
#include <boost/asio.hpp>
#include <set>
#include <thread>

#include <wfc/inet/context.hpp>
#include <wfc/inet/connection_base.hpp>
#include <wfc/inet/connection.hpp>
#include <wfc/inet/connection_manager.hpp>
#include <wfc/inet/connection_aspect.hpp>
#include <wfc/inet/server_aspect.hpp>
#include <wfc/inet/server_context.hpp>
#include <wfc/inet/connection_context.hpp>
#include <wfc/inet/work_thread.hpp>
#include <wfc/inet/ad_connection_handle.hpp>
#include <wfc/inet/accept_thread.hpp>
#include <wfc/inet/ad_acceptor.hpp>
#include <wfc/inet/ad_st_acceptor.hpp>
#include <wfc/inet/ad_mt_acceptor.hpp>

namespace wfc{ namespace inet{



struct aspect_server: fas::aspect< fas::type_list_n<
  context<server_context>,
  fas::advice<_connection_handle_,  ad_connection_handle>, 
  fas::advice<_acceptor_, ad_acceptor>,
  fas::advice<_st_acceptor_, ad_st_acceptor>,
  fas::advice<_mt_acceptor_, ad_mt_acceptor>,
  fas::alias<_start_, _acceptor_>,
  fas::alias<_stop_, _acceptor_>,
  //fas::group<_startup_,  _start_>, 
  fas::group<_startup_,  _connection_handle_>
>::type>
{
};

struct aspect_connection: fas::aspect< fas::type_list_n<
  context<connection_context>
>::type>
{
};


struct aspect_traits: fas::aspect< fas::type_list_n<
  connection_aspect< aspect_connection >,
  server_aspect< aspect_server >,
  connection_base< fas::aspect_class >,
  connection< basic_connection >
>::type> 
{
};


template<typename A>
struct server_traits
{
  typedef typename fas::merge_aspect< A, aspect_traits>::type aspect_type;
  typedef typename fas::aspect_class<aspect_type>::aspect aspect;
  
  ///
  /// connection
  ///
  typedef typename aspect::template advice_cast<_connection_aspect_>::type connection_aspect_type;
  
  template<typename ConnAspect>
  using connection_base_t = typename aspect::template advice_cast<_connection_base_>::type::template type<ConnAspect>;
  
  template<typename ConnAspect, template<typename> class ConnBase >
  using connection_t = typename aspect::template advice_cast<_connection_>::type::template type<ConnAspect, ConnBase>;
  
  typedef connection_t<connection_aspect_type, connection_base_t> connection_type;
  typedef typename connection_type::aspect::template advice_cast<_context_>::type connection_context_type;
  
  ///
  /// server
  ///
  typedef typename aspect::template advice_cast<_server_aspect_>::type server_aspect_type;

  typedef fas::aspect_class<server_aspect_type> server_base;
  typedef typename server_base::aspect::template advice_cast<_context_>::type server_context_type;

};

  
template<typename A = fas::aspect<> >
class server:
  public server_traits<A>::server_base
{
  typedef server_traits<A> traits;
  
public:
  typedef typename traits::server_base super;
  typedef typename traits::server_context_type server_context_type;
  
  typedef typename traits::connection_type connection_type;
  typedef typename traits::connection_context_type connection_context_type;
  
public:
  
  typedef typename super::aspect aspect;
  
  server( ::boost::asio::io_service& io_service)
    : _io_service(io_service)
  {
  }
  
  void start()
  {
    this->get_aspect().template getg<_startup_>()(*this, fas::tag<_startup_>() );
    this->get_aspect().template get<_start_>()(*this, fas::tag<_start_>() );
  }
  
  void stop()
  {
    this->get_aspect().template get<_stop_>()(*this, fas::tag<_stop_>() );
  }
  
  server_context_type& server_context() { return this->get_aspect().template get<_context_>();}
  const server_context_type& server_context() const { return this->get_aspect().template get<_context_>();}

  // TODO: сделать mutex и реконфиг
  connection_context_type& connection_context() { return _connection_context;}
  const connection_context_type& connection_context() const { return _connection_context;}
  
  boost::asio::io_service& get_io_service()
  {
    return _io_service;
  }
  
  std::shared_ptr<connection_type> create_connection()
  {
    std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>();
    pconn->context() = _connection_context;
    return pconn;
  }
  
private:
  ::boost::asio::io_service& _io_service;
  connection_context_type _connection_context;
};

}}