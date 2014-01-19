#pragma once

#include <wfc/inet/srv/server_helper.hpp>
#include <wfc/inet/server_config.hpp>
#include <wfc/core/global.hpp>
#include <wfc/io_service.hpp>
#include <fas/aop.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{
  
template<typename... Args>
class server:
  public server_helper<Args...>::server_base
{
  typedef server_helper<Args...> helper;
  
public:
  typedef typename helper::server_base super;
  typedef typename helper::server_context_type server_context_type;
  
  typedef typename helper::connection_type connection_type;
  typedef typename helper::connection_context_type connection_context_type;
  
  typedef typename helper::config_type config_type;
  typedef typename helper::socket_type socket_type;
  
  typedef typename helper::connection_manager_type connection_manager_type;
public:
  
  typedef typename super::aspect aspect;
  
  server( ::wfc::io_service& io_service, const config_type& conf )
    : _io_service(io_service)
  {
    this->get_aspect().template get<_configuration_>()(*this, conf);
  }
  
  server( std::weak_ptr< wfc::global > g, const config_type& conf )
    : server( *(g.lock()->io_service.lock()), conf)
  {
    this->get_aspect().template get<_configuration_>()(*this, conf);
  }
  
  void reconfigure(const config_type& conf)
  {
    this->get_aspect().template get<_configuration_>()(*this, conf);
  }
  
  void initialize()
  {
  }
  
  void start()
  {
    _connection_manager = std::make_shared<connection_manager_type>(); // TODO: перенести в адвайс
    this->get_aspect().template getg<_startup_>()(*this, fas::tag<_startup_>() );
    this->get_aspect().template getg<_start_>()(*this, fas::tag<_start_>() );
  }
  
  void stop()
  {
    this->get_aspect().template getg<_stop_>()(*this, fas::tag<_stop_>() );
  }
  
  server_context_type server_context() const
  {
    return this->get_aspect().template get<_context_>();
  }
  
  void server_context(const server_context_type& value) 
  {
    this->get_aspect().template get<_context_>() = value;
  }

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
  
  template< typename ...ConnArgs>
  std::shared_ptr<connection_type> create_connection( ConnArgs&& ...args )
  {
    std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>( std::forward<ConnArgs>(args)... );
    pconn->context() = _connection_context;
    return pconn;
  }
  
  std::shared_ptr<connection_manager_type> connection_manager()
  {
    return _connection_manager;
  }
  
private:
  ::wfc::io_service& _io_service;
  connection_context_type _connection_context;
  std::shared_ptr<connection_manager_type> _connection_manager;
};

}}