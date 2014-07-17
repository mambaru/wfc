

#include <wfc/io/ip/tcp/rn/acceptor.hpp>
#include <wfc/io/ip/tcp/rn/acceptor_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

acceptor::~acceptor()
{
  //_impl.reset();
}

  
acceptor::acceptor(std::shared_ptr<acceptor_impl> impl)
  : _impl( impl ) 
{
}

  
acceptor::acceptor(acceptor::descriptor_type&& desc, const acceptor::options_type& conf)
  : _impl( std::make_shared<acceptor_impl>(std::move(desc), conf) )
{
}

std::shared_ptr<acceptor> acceptor::clone(::wfc::io_service& io)
{
  typedef typename descriptor_type::protocol_type protocol_type;
  auto impl = _impl->clone<acceptor_impl>( io, protocol_type::v4() );
  return std::shared_ptr<acceptor>( new acceptor( impl ) );
}

void acceptor::reconfigure(const options_type& )
{
  abort();
}

void acceptor::listen()
{
  _impl->listen();
}

void acceptor::start()
{
  _impl->start();
}

void acceptor::close()
{
  
  if ( _impl->descriptor().is_open() )
  {
    _impl->descriptor().close();
  }
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
