#include <comet/asio/server.hpp>
#include <boost/asio.hpp>
#include <signal.h>
using namespace ::mamba::comet;


struct _start_;
struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "START!" << std::endl;
    std::string str;
    for (int i =0; i < 100; ++i)
    {
      str="[[[";
      t.write(str.c_str(), str.size());
      std::stringstream ss;
      for (int k =0; k < 100; ++k)
        ss << "Привет Чувак "<< i<<"-" << k <<",";
      ss << "]]]\r\n";
      str = ss.str();
      t.write(str.c_str(), str.size());
    }
    std::cout << "ALL WRITE!" << std::endl;
  }
};

typedef inet::tcp_server<
    inet::echo_connection<
      fas::aspect< fas::type_list_n<
        fas::group<inet::_start_, _start_>,
        fas::advice<_start_, ad_start>
      >::type>
    >
  > server_type;
typedef server_type::connection_type connection_type;

std::shared_ptr<connection_type> conn;


int main(int argc, char* argv[])
{
  inet::server_config conf;
  conf.enabled = true;
  conf.host = "0.0.0.0";
  conf.port = "12345";

  
  ::boost::asio::io_service io_service;
  ::boost::asio::signal_set ss(io_service);
  server_type srv(io_service);

  ss.add(SIGINT);
  ss.add(SIGTERM);

  ss.async_wait(
      [&srv](boost::system::error_code /*ec*/, int /*signo*/)
      {
        std::cout << "stop..." << std::endl;
        srv.stop();
        std::cout << "stop! Done!" << std::endl;
      });

  
  srv.start(conf);
  io_service.run();
  std::cout << "STOP" << std::endl;
  std::cin.get();
  // conn.write("final!", 6);
  return 0;
}
