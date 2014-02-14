#include <boost/asio.hpp>
#include <wfc/inet/connection.hpp>
#include <wfc/inet/conn/aspect.hpp>
#include <wfc/inet/tags.hpp>
#include <wfc/inet/context.hpp>


typedef fas::aspect<
  wfc::inet::context<int>,
  wfc::inet::conn::aspect
  // fas::type< wfc::inet::_socket_type_, boost::asio::ip::tcp::socket>
> connection_aspect;


int main()
{
  typedef wfc::inet::connection< connection_aspect > connection_type;
  connection_type conn;
  conn.context() = 10;
  auto conn1 = conn.clone();
  
  conn1->start();
  //conn1->get_io_service();
  if ( conn1->context()!=10 )
  {
    std::cout << conn.context()   << std::endl;
    std::cout << conn1->context() << std::endl;
    abort();
  }
  return 0;
}