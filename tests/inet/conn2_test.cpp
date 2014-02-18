#include <boost/asio.hpp>
#include <wfc/inet/connection.hpp>
#include <wfc/inet/conn/stream/aspect.hpp>
#include <wfc/inet/tags.hpp>
#include <wfc/inet/context.hpp>


typedef fas::aspect<
  wfc::inet::context<int>,
  wfc::inet::conn::stream::aspect
> connection_aspect;

struct conf 
{
  bool enable_stat;
};

int main()
{
  typedef wfc::inet::connection< connection_aspect > connection_type;
  connection_type conn;
  conn.context() = 10;
  
  conn.context();
  conn.socket();
  conn.dispatch([](){});
  conn.post([](){});
  conn.configure(conf());
  conn.initialize(fas::empty_type());
  conn.start();
  conn.stop();
  auto conn1 = conn.clone();
  conn1->start();
  
  if ( conn1->context()!=10 )
  {
    std::cout << conn.context()   << std::endl;
    std::cout << conn1->context() << std::endl;
    abort();
  }
  return 0;
}