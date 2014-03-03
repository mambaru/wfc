#include <iostream>

/*
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/strategy/posix/reader/sync_loop.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>
#include <wfc/io/strategy/posix/reader/config.hpp>
*/

#include <wfc/io/reader/reader.hpp>
#include <wfc/io/types.hpp>

#include <thread>
#include <string>
#include <boost/asio.hpp>

/*
typedef wfc::io::strategy::posix::reader::config init_info;
*/

std::vector<std::string> result_list;

/*
struct options
{
  wfc::io::handler handler = [](wfc::io::data_ptr d, wfc::io::callback)
  {
    std::cout << "handle " << std::string(d->begin(), d->end() ) << std::endl;
    result_list.push_back( std::string(d->begin(), d->end() ) );
  };
  
  size_t input_buffer_size = 8096;
  std::function<void()> not_alive = nullptr;
};
*/

/*

struct ad_handler
{
  template<typename T>
  void operator()(T& , typename T::data_ptr d)
  {
    std::cout << "handle " << std::string(d->begin(), d->end() ) << std::endl;
    result_list.push_back( std::string(d->begin(), d->end() ) );
  }
};
*/

struct reader_aspect
  : fas::aspect<
      wfc::io::reader::stream< boost::asio::posix::stream_descriptor >,
      wfc::io::reader::error_log
    >
{};


int main()
{
  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  //boost::asio::io_service::work wrk(*io_service);
  wfc::io::reader::options init;
  init.input_buffer_size = 8096;
  init.not_alive = nullptr;
  init.handler = [](wfc::io::data_ptr d, wfc::io::callback)
  {
    std::cout << "handle " << std::string(d->begin(), d->end() ) << std::endl;
    result_list.push_back( std::string(d->begin(), d->end() ) );
  };
  
  
  {
    typedef wfc::io::reader::reader< reader_aspect > reader_type;

    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    
    std::cout << "thread..." << std::endl;
    std::thread th([&reader, &io_service]()
    {
      reader.start();
      io_service->run();
    });
    usleep(100000);
    write(dd[1], "test1", 5);
    usleep(100000);
    write(dd[1], "test2", 5);
    usleep(100000);
    close(dd[0]);
    close(dd[1]);
    io_service->stop();
    th.join();
  }
  
  
  if ( result_list.size()!=2 )
    abort();
  if ( result_list[0]!="test1" )
    abort();
  if ( result_list[1]!="test2" )
    abort();
}
