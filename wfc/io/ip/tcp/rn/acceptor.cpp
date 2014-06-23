

#include <wfc/io/ip/tcp/rn/acceptor.hpp>
#include <wfc/io/ip/tcp/rn/acceptor_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

acceptor::~acceptor()
{
  //_impl.reset();
}

  
acceptor::acceptor(acceptor::descriptor_type&& desc, const acceptor::options_type& conf/*, wfc::io::incoming_handler handler*/ )
  : _impl( std::make_shared<acceptor_impl>(std::move(desc), conf/*, handler*/) )
{
  
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
