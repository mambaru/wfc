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
  
  /*
  void configure(const config_type& conf)
  {
    //this->get_aspect().template get<_configuration_>()(*this, conf);
    
    std::cout << "server configure" << std::endl;
    server_context_type cntx = this->server_context();
    cntx.enable_stat = conf.enable_stat;
    cntx.host = conf.host;
    cntx.port = conf.port;
    this->server_context(cntx);
    
  }*/
  
  void reconfigure(const config_type& conf)
  {
    this->get_aspect().template get<_configuration_>()(*this, conf);
    //this->configure(conf);
    // TODO: _reconfigure_
  }
  
  void initialize()
  {
    // TODO: stat
  }
  
  void start()
  {
    this->get_aspect().template getg<_startup_>()(*this, fas::tag<_startup_>() );
    this->get_aspect().template getg<_start_>()(*this, fas::tag<_start_>() );
  }
  
  void stop()
  {
    std::cout << "server::stop" << std::endl;
    this->get_aspect().template getg<_stop_>()(*this, fas::tag<_stop_>() );
    std::cout << "server::stoped!!!" << std::endl;
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
  //server_config _config;
  //std::shared_ptr< ::wfc::io_service > _io_service;
  ::wfc::io_service& _io_service;
  connection_context_type _connection_context;
};

}}