#pragma once

#include <wfc/inet/srv/server_helper.hpp>
#include <fas/aop.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{
  
template<typename A = fas::aspect<> >
class server:
  public server_helper<A>::server_base
{
  typedef server_helper<A> helper;
  
public:
  typedef typename helper::server_base super;
  typedef typename helper::server_context_type server_context_type;
  
  typedef typename helper::connection_type connection_type;
  typedef typename helper::connection_context_type connection_context_type;
  
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
  
  // TODO: reconfigure
  server_context_type server_context() const
  {
    return this->get_aspect().template get<_context_>();
  }
  
  void server_context(const server_context_type& value) 
  {
    this->get_aspect().template get<_context_>() = value;
  }

  // TODO: сделать mutex и реконфиг
  connection_context_type connection_context() const
  {
    return _connection_context;
  }
  
  void connection_context(const connection_context_type& value)
  {
    _connection_context = value;
  }
  
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