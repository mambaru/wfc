

#include <wfc/io/ip/tcp/rn/acceptor.hpp>
#include <wfc/io/ip/tcp/rn/acceptor_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

acceptor::~acceptor()
{
  //_impl.reset();
}

  
acceptor::acceptor(acceptor::descriptor_type&& desc, const acceptor::options_type& conf)
  : _impl( std::make_unique<acceptor_impl>(std::move(desc), conf) )
{
  
}

void acceptor::start()
{
  _impl->start();
}

void acceptor::stop()
{
  _impl->stop();
}
  
void acceptor::shutdown()
{
  
}

  

  
}}}}}
