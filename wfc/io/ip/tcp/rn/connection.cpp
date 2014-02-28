

#include <wfc/io/ip/tcp/rn/connection.hpp>
#include <wfc/io/ip/tcp/rn/connection_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

connection::~connection()
{
  //_impl.reset();
}

  
connection::connection(connection::descriptor_type&& desc, const connection::options_type& conf)
  : _impl( std::make_unique<connection_impl>(std::move(desc), conf) )
{
  
}

void connection::start()
{
  _impl->start();
}

void connection::stop()
{
  _impl->stop();
}
  
void connection::shutdown()
{
  
}

  

  
}}}}}
