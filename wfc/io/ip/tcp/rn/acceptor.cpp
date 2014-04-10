

#include <wfc/io/ip/tcp/rn/acceptor.hpp>
#include <wfc/io/ip/tcp/rn/acceptor_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

acceptor::~acceptor()
{
  //_impl.reset();
}

  
acceptor::acceptor(acceptor::descriptor_type&& desc, const acceptor::options_type& conf, wfc::io::handler handler )
  : _impl( std::make_unique<acceptor_impl>(std::move(desc), conf, handler) )
{
  
}

void acceptor::start()
{
  _impl->start();
}

void acceptor::stop()
{
  _impl->stop();
  std::cout << "void acceptor::stop() reset..." << std::endl;
  _impl.reset();
  std::cout << "...void acceptor::stop()" << std::endl;
}
  
void acceptor::shutdown()
{
  
}

  

  
}}}}}
