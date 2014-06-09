#include <iostream>


/*
#include <wfc/io/connection/connection.hpp>
#include <wfc/io/strategy/posix/writer/config.hpp>
#include <wfc/io/strategy/posix/connection/rn/stream.hpp>
*/

#include <wfc/io/connection/connection.hpp>
#include <wfc/io/ip/tcp/rn/connection_aspect.hpp>
#include <boost/asio.hpp>
#include <string>
#include <thread>


/*
struct ad_reverse
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d)
  {
    std::reverse( d->begin(), d->end() );
    t.get_aspect().template get<wfc::io::_outgoing_>()(t, std::move(d) );
  }
};*/

struct config
{
    size_t output_buffer_size = 1024;
    size_t input_buffer_size = 1024;
    std::function<void()> not_alive = nullptr;
};

  
struct connection_aspect: 
  fas::aspect<
    fas::type< wfc::io::_descriptor_type_, boost::asio::local::stream_protocol::socket>, 
    wfc::io::ip::tcp::rn::connection_aspect
    /*
    fas::advice< wfc::io::_options_type_, config>,
    fas::advice< wfc::io::_incoming_, ad_reverse>,
    fas::type< wfc::io::_descriptor_type_, boost::asio::local::stream_protocol::socket>,
    wfc::io::strategy::posix::connection::rn::stream
    */
  >
{
};

typedef wfc::io::connection::connection<connection_aspect> connection_type;

//typedef wfc::io::ip::tcp::rn::connection connection_type;

int main()
{
  boost::asio::io_service io_service;
  boost::asio::io_service::work wrk(io_service);
  
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
  
  connection_type::options_type conf;
  conf.incoming_handler = wfc::io::simple_handler( [](wfc::io::data_ptr d, wfc::io::outgoing_handler_t callback) {
    std::cout << "------handler-------" << std::endl;
    std::reverse(d->begin(), d->end());
    callback( std::move(d) );
  });
  
  connection_type conn( std::move(sock), conf);
  conn.start();
  
   
  std::cout << "-4-" << std::endl;
  std::string income_string = "Hello World\r\n";
  sock1.write_some( boost::asio::buffer(income_string));
  
  sleep(1);
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
}

