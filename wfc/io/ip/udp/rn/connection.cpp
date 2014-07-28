

#include <wfc/io/ip/udp/rn/connection.hpp>
#include <wfc/io/ip/udp/rn/connection_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

connection::~connection()
{
}

connection::connection( std::shared_ptr<connection_impl> impl)
  : _impl(impl)
{
  DEBUG_LOG_MESSAGE("connection::connection( std::shared_ptr<connection_impl> impl)")
}

  
connection::connection(descriptor_type&& desc, const options_type& conf )
  : _impl( std::make_shared<connection_impl>(std::move(desc), conf) )
{
}

std::shared_ptr<connection> connection::clone(::wfc::io_service& io)
{
  DEBUG_LOG_MESSAGE("connection::clone() -1-")
  typedef typename descriptor_type::protocol_type protocol_type;
  auto impl = _impl->clone<connection_impl>( io, protocol_type::v4() );
  DEBUG_LOG_MESSAGE("connection::clone() -2-")
  return std::shared_ptr<connection>( new connection( impl ) );
}

void connection::reconfigure( const options_type& /*conf*/ )
{
  //_impl->reconfigure(conf);
}

io_id_t connection::get_id() const
{
  return _impl->get_id();
}

void connection::start()
{
  DEBUG_LOG_MESSAGE( "udp connection::listen() -1-" )
  _impl->start();
  DEBUG_LOG_MESSAGE( "udp connection::listen() -2-" )
}

void connection::close()
{
  //_impl->close();
}

void connection::stop(std::function<void()> finalize)
{
  DEBUG_LOG_MESSAGE( "udp void connection::stop -1-" )
  _impl->descriptor().cancel();
  DEBUG_LOG_MESSAGE( "udp void connection::stop -2-" )
  _impl->stop(finalize);
  DEBUG_LOG_MESSAGE( "udp void connection::stop -3-" )
}

void connection::listen()
{
  DEBUG_LOG_MESSAGE( "udp connection::listen() -1-" )
  _impl->listen();
  DEBUG_LOG_MESSAGE( "udp connection::listen() -2-" )
}

const wfc::io_service::strand& connection::strand() const
{
  return _impl->strand();
}

wfc::io_service::strand& connection::strand()
{
  return _impl->strand();
}

::wfc::io_service& connection::get_io_service()
{
  return _impl->get_io_service();
}

void connection::shutdown()
{
  
}

}}}}}
