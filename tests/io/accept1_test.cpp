#include <iostream>


#include <wfc/io/acceptor/acceptor.hpp>
#include <wfc/io/strategy/ip/tcp/acceptor/async.hpp>
#include <boost/asio.hpp>
#include <string>
#include <thread>

class 

struct ad_ready
{
  template<typename T>
  void operator()( T& , typename T::data_ptr )
  {
    std::cout << "NEW SOKET" << std::endl;
    
  }
};

  
struct acceptor_aspect: 
  fas::aspect<
    fas::advice< wfc::io::_incoming_, ad_ready>,
    wfc::io::strategy::ip::tcp::acceptor::async
  >
{};

typedef wfc::io::acceptor::acceptor<acceptor_aspect> acceptor_type;

struct config
{
    size_t output_buffer_size = 1024;
    size_t input_buffer_size = 1024;
    std::function<void()> not_alive = nullptr;
};

int main()
{
  boost::asio::io_service io_service;
  boost::asio::io_service::work wrk(io_service);
  
  typedef acceptor_type::descriptor_type descriptor_type;
  
  descriptor_type desc( io_service );
  
    boost::asio::ip::tcp::resolver resolver( io_service );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      "0.0.0.0", 
      "12345"
    });

    desc.open(endpoint.protocol());
    desc.set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
    desc.bind(endpoint);
    desc.listen();

  
  config conf;
  acceptor_type acceptor( std::move(desc), conf );
  
  acceptor.start();
  io_service.run();
  /*
  std::cout << "-1-" << std::endl;
  ::unlink("/tmp/my_cool_app"); 
  boost::asio::local::stream_protocol::socket sock1(io_service);
  std::thread th( [&io_service, &sock1]()
  {
    std::cout << "-2-" << std::endl;
    boost::asio::local::stream_protocol::endpoint ep1("/tmp/my_cool_app");
    sock1.connect(ep1);
    std::cout << "-2.1-" << std::endl;
    io_service.run();
  });

  boost::asio::local::stream_protocol::endpoint ep("/tmp/my_cool_app");
  boost::asio::local::stream_protocol::acceptor acceptor(io_service, ep);
  boost::asio::local::stream_protocol::socket sock(io_service);
  acceptor.accept(sock);
  
  std::cout << "-3-" << std::endl;
  
  config conf;
  
  connection_type conn( std::move(sock), conf );
  conn.start();
  
   
  std::cout << "-4-" << std::endl;
  std::string income_string = "Hello World\r\n";
  sock1.write_some( boost::asio::buffer(income_string));
  
  std::cout << "-5-" << std::endl;
  std::vector<char> result_data;
  result_data.resize(100);
  size_t size = sock1.read_some( boost::asio::buffer(result_data, 100) );
  
  std::cout << "-6-" << std::endl;
  
  std::string result_string(result_data.begin(), result_data.end());
  
  std::cout << size << "Done! [" << result_string << "]" << std::endl;
  io_service.stop();
  th.join();
  std::cout << size << "Final!" << std::endl;
  */
}

