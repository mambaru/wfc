#include <wfc/inet/server.hpp>
#include <wfc/inet/echo/aspect_stream_rn.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

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
  fas::aspect< 
    wfc::inet::connection_aspect< wfc::inet::echo::aspect_stream_rn > 
  > 
{
};

::boost::asio::io_service io_service;

void client_test()
{
  std::thread([](){
    sleep(1);
    ::boost::asio::io_service io_service;
    std::cout << "1 client " << std::endl;
    ::boost::asio::ip::tcp::socket s(io_service);
    ::boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::connect(s, resolver.resolve({"192.168.2.166", "12345"}));
    std::cout << "2 client " << std::endl;
    
    std::string str1 = "1234567890\r\nqwertyuiop\r\n1234567890XYZ\r\nйцукен\r\n";
    auto beg = str1.begin();
    auto end = str1.end();
    while (beg!=end)
    {
      std::cout << "3 client " << std::endl;
      auto cur = beg;
      if ( std::distance(beg, end) < 1 )
        cur = end;
      else
        cur += 1;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::vector<char> arr(  beg, cur );
      boost::asio::write(s, boost::asio::buffer(arr, arr.size() ));
      //boost::asio::write(s, boost::asio::buffer(str1, str1.size() ));
      //break;
      beg = cur;
    }
    

    char reply[1024];
    std::cout << "4 client " << std::endl;
    size_t reply_length = boost::asio::read(s, boost::asio::buffer(reply, str1.size()) );
    s.close();
    std::cout << "5 client " << std::endl;
    std::string str2( reply, reply + reply_length);
    std::cout << str1 << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << str2 << std::endl;
    ::io_service.stop();

  }).detach();
}

int main(int argc, char */*argv*/[])
{
  
  wfc::inet::server< common_aspect > srv(io_service, wfc::inet::server_tcp_config() );
  auto conn_conf = srv.connection_context();
  conn_conf.enable_stat = true;
  srv.connection_context(conn_conf);
  
  auto config = srv.server_context();
  config.host = "192.168.2.166";
  config.port = "12345";
  config.listen_threads = 3;
  config.worker_threads = 3;
  srv.server_context(config);
  srv.start();
  if ( argc==1)
    client_test();
  //for (;;)
  std::cout << "run" << std::endl;
  io_service.run();
  std::cout << "FINAL" << std::endl;
  std::cin.get();
  srv.stop();
  
}