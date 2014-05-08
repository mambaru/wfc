#pragma once 

#include <wfc/inet/client/tags.hpp>
#include <wfc/inet/client/client_helper.hpp>
#include <wfc/inet/tags.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace inet{
  
template<typename... Args>
class client
  : public client_helper<Args...>::client_base
{
  typedef client_helper<Args...> helper;
public:
  typedef typename helper::client_base super;
  typedef typename helper::connection_type connection_type;
  //typedef std::shared_ptr<connection_type> connection_ptr;
  typedef typename helper::socket_type  socket_type;
  typedef typename helper::connection_context_type connection_context_type;
  typedef typename helper::client_context_type client_context_type;
  typedef typename helper::config_type config_type;
  typedef typename helper::connection_manager_type connection_manager_type;
  

  /*
  client(::boost::asio::io_service& io)
    : _socket( std::make_shared<socket_type>(io) )
  {
  }
  */
  
  client( ::wfc::io_service& io_service, const config_type& conf )
    : _io_service(io_service)
  {
    this->get_aspect().template get<srv::_configurator_>()(*this, conf);
  }
  
  client( std::weak_ptr< wfc::global > g, const config_type& conf )
    : client( *(g.lock()->io_service.lock()), conf)
  {
    this->get_aspect().template get<srv::_configurator_>()(*this, conf);
  }
  
  void reconfigure(const config_type& conf)
  {
    this->get_aspect().template get<srv::_configurator_>()(*this, conf);
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
  
  client_context_type client_context() const
  {
    return this->get_aspect().template get<_context_>();
  }
  
  void client_context(const client_context_type& value) 
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
  
  std::weak_ptr<connection_type>  connection()
  {
    //return this->get_aspect().template get<_connection_manager_>()->least();
    return _connection_manager->least();
  }
  
  
  virtual void send(data_ptr d) 
  {
    //this->get_aspect().template get<_connection_manager_>()->least()->send( std::move(d) );
    if ( auto conn = this->connection().lock() )
      conn->send( std::move(d) );
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
  
  //std::unique_ptr<connection_type> _connection;
};

}}