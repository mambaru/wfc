#include <wfc/inet/server.hpp>
#include <boost/asio.hpp>

struct connection_aspect: wfc::inet::connection_aspect< fas::type_list_n<
  wfc::inet::context< wfc::inet::context_connection >,
  fas::alias<wfc::inet::basic::_incoming_, ::wfc::inet::basic::_outgoing_>
>::type>
{};

struct common_aspect: fas::aspect<fas::type_list_n<
  connection_aspect
>::type> 
{
  
};

int main()
{
  ::boost::asio::io_service io_service;
  wfc::inet::server< common_aspect > srv(io_service);
  srv.connection_context().enable_stat = true;
  srv.server_context().host = "0.0.0.0";
  srv.server_context().port = "12345";
  srv.server_context().listen_threads = 10;
  srv.start();
  //for (;;)
    io_service.run();
  std::cout << "FINAL" << std::endl;
  std::cin.get();
  
}