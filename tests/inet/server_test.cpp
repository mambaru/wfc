#include <wfc/inet/server.hpp>
#include <wfc/inet/echo/aspect_stream_rn.hpp>
#include <boost/asio.hpp>

/*
struct connection_aspect: wfc::inet::connection_aspect< fas::aspect<
  wfc::inet::context< wfc::inet::connection_context >,
  fas::alias<wfc::inet::basic::_incoming_, ::wfc::inet::basic::_outgoing_>
>
>
{};
*/

/*
struct connection_aspect: fas::aspect<
  wfc::inet::context< wfc::inet::connection_context >,
  fas::alias< wfc::inet::stream::_incoming_, ::wfc::inet::stream::_outgoing_>,
  wfc::inet::stream::aspect
>{};
*/

struct common_aspect: 
  fas::aspect< wfc::inet::connection_aspect< wfc::inet::echo::aspect_stream_rn > > 
{
};

int main()
{
  ::boost::asio::io_service io_service;
  wfc::inet::server< common_aspect > srv(io_service, wfc::inet::server_tcp_config() );
  auto conn_conf = srv.connection_context();
  conn_conf.enable_stat = true;
  srv.connection_context(conn_conf);
  
  auto config = srv.server_context();
  config.host = "127.0.0.1";
  config.port = "12345";
  config.listen_threads = 0;
  config.worker_threads = 0;
  srv.server_context(config);
  srv.start();
  //for (;;)
    io_service.run();
  std::cout << "FINAL" << std::endl;
  std::cin.get();
  srv.stop();
  
}