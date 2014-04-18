#include <wfc/io/ip/tcp/rn/server.hpp>
#include <wfc/io_service.hpp>
#include <wfc/io/types.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <algorithm>

void test(wfc::io_service& io_service)
{
  std::cout << "run" << std::endl;
  io_service.run();
  std::cout << "done" << std::endl;
}

typedef wfc::io::ip::tcp::rn::server server;

void handler( wfc::io::data_ptr d, wfc::io::callback callback)
{
  std::reverse( d->begin(), d->end() );
  callback( std::move(d) );
}

int main(int argc, char* /*argv*/[])
{
  wfc::io_service io_service;

  /*
  boost::asio::ip::udp::resolver resolver( io_service );
  boost::asio::ip::udp::endpoint endpoint = *resolver.resolve({
      "0.0.0.0", 
      "12345"
  });

  
  boost::asio::ip::udp::socket srv1(io_service, boost::asio::ip::udp::v4());
  srv1.bind(endpoint);
  std::thread([&srv1]() {
    for (int i=0;i<2;++i)
    {
    boost::asio::ip::udp::endpoint endpoint;
    char ch;
    srv1.receive_from( boost::asio::buffer(&ch, 1),  endpoint);
    std::cout << 'A' << std::endl;
    srv1.connect(endpoint);
    std::cout << srv1.local_endpoint().port() << std::endl;
    std::cout << srv1.remote_endpoint().port() << std::endl;
    std::cout << endpoint.port() << std::endl;
    }
    exit(1);
  }).detach();
  boost::asio::ip::udp::socket cli(io_service, boost::asio::ip::udp::v4());
  char ch='A';
  cli.send_to(boost::asio::buffer(&ch, 1), endpoint );
  boost::asio::ip::udp::socket cli2(io_service, boost::asio::ip::udp::v4());
  cli2.send_to(boost::asio::buffer(&ch, 1), endpoint );
  */
  
  server::options_type conf;
  conf.host = "0.0.0.0";
  conf.port = "12345";
  //conf.acceptors = 1;
  conf.threads = 4;
  //conf.handler = handler;
  wfc::io_service::work wrk(io_service);
  
  server srv(io_service, conf, wfc::io::simple_handler(handler) );
  
  srv.start();
  
  std::thread th;
  
  if (argc == 1)
    th = std::thread( std::bind(test, std::ref(io_service) ) );
  
  ::sleep(1);
  //io_service.run();
  
  
  std::cout << "stop..." << std::endl;
  srv.stop();
  std::cout << "stop io_service" << std::endl;
  io_service.stop();
  std::cout << "...stop Done" << std::endl;
  
  if (argc == 1)
    th.join();
}
