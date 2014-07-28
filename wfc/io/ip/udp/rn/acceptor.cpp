

#include <wfc/io/ip/udp/rn/acceptor.hpp>
#include <wfc/io/ip/udp/rn/connection.hpp>
//#include <wfc/io/ip/udp/rn/acceptor_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

// TODO: Нафиг удалить 
  
acceptor::~acceptor()
{
  //_impl.reset();
}

  
acceptor::acceptor(std::shared_ptr<connection> impl)
  : _impl( impl ) 
{
}

  
acceptor::acceptor(acceptor::descriptor_type&& desc, const acceptor::options_type& conf)
  : _impl( std::make_shared<connection>(std::move(desc), conf /*.connection*/ ) )
  , _options(conf)
{
}

std::shared_ptr<acceptor> acceptor::clone( ::wfc::io_service& io )
{
  typedef typename descriptor_type::protocol_type protocol_type;
  auto impl = _impl->clone/*<connection>*/( io /*, protocol_type::v4()*/ );
  return std::shared_ptr<acceptor>( new acceptor( impl ) );
}

void acceptor::reconfigure(const options_type& )
{
  abort();
}

void acceptor::listen()
{
  COMMON_LOG_MESSAGE("fuckup udp \"listen\" " << _options.host << ":" << _options.port)
  //_impl->listen();
  /*
      COMMON_LOG_MESSAGE("udp \"listen\" " << _options.host << ":" << _options.port)
    
    boost::asio::ip::udp::resolver resolver( super::get_io_service() );
    boost::asio::ip::udp::endpoint endpoint = *resolver.resolve({
      _options.host, 
      _options.port
    });

    _impl->descriptor().open(endpoint.protocol());
    _impl->descriptor().bind(endpoint);
    */
    

}

void acceptor::start()
{
  DEBUG_LOG_MESSAGE( "##################################### void acceptor::start()" )
  _impl->start();
}

void acceptor::close()
{
  /*
  if ( _impl->descriptor().is_open() )
  {
    _impl->descriptor().close();
  }
  */
}

void acceptor::stop(std::function<void()> finalize)
{
  _impl->stop(finalize);
  _impl.reset();
}
  
void acceptor::shutdown()
{
  
}

  
}}}}}
