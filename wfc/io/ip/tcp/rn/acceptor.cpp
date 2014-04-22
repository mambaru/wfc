

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

void acceptor::stop(std::function<void()> finalize)
{
  std::cout << "---- void acceptor::stop() ---" << std::endl;
  _impl->stop(finalize);
  std::cout << "void acceptor::stop() 1" << std::endl;
  _impl.reset();
  std::cout << "void acceptor::stop() Done" << std::endl;
  /*
  std::cout << "void acceptor::stop() ------ " << std::endl;
  _impl->stop();
  std::cout << "void acceptor::stop() reset..." << std::endl;
  auto &stg = _impl->get_aspect().get<_holder_storage_>();
  for(auto& conn : stg)
  {
    std::cout << "void acceptor::stop() -1- --------------------------------- connection stop" << std::endl;
    conn.second->stop();
    std::cout << "void acceptor::stop() -1.s-" << std::endl;
    conn.second.reset();

  }
  std::cout << "void acceptor::stop() -2-" << std::endl;
  stg.clear();
  std::cout << "void acceptor::stop() -3-" << std::endl;
  _impl.reset();
  std::cout << "...void acceptor::stop()" << std::endl;
  */
  
}
  
void acceptor::shutdown()
{
  
}

  

  
}}}}}
