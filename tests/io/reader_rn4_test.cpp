#include <iostream>

#include <wfc/io/reader/reader.hpp>
#include <wfc/io/strategy/posix/reader/rn/sync_loop.hpp>
#include <wfc/io/strategy/posix/reader/rn/log.hpp>
#include <wfc/io/strategy/posix/reader/rn/trace.hpp>
#include <wfc/io/strategy/posix/reader/rn/incoming_buffer_check.hpp>
#include <wfc/io/strategy/posix/reader/rn/config.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>

#include <boost/asio.hpp>
#include <thread>

typedef wfc::io::strategy::posix::reader::rn::config init_info;

std::vector<std::string> result_list;

struct _handler_;
struct ad_handler
{
  template<typename T>
  void operator()(T& , typename T::data_ptr d)
  {
    std::cout << "handle [" << std::string(d->begin(), d->end() ) << "]" << std::endl;
    result_list.push_back( std::string(d->begin(), d->end() ) );
  }
};

struct reader_aspect: 
  fas::aspect<
    fas::advice< wfc::io::reader::_outgoing_, ad_handler>,
    wfc::io::strategy::posix::reader::trace,
    wfc::io::strategy::posix::reader::log,
    wfc::io::strategy::posix::reader::rn::trace,
    wfc::io::strategy::posix::reader::rn::log,
    wfc::io::strategy::posix::reader::rn::incoming_buffer_check,
    wfc::io::strategy::posix::reader::rn::sync_loop
  >
{};


int main()
{
  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  boost::asio::io_service::work wrk(*io_service);
  init_info init;
  init.input_buffer_size = 8096;
  init.not_alive = nullptr;
  {
    typedef wfc::io::reader::reader< reader_aspect > reader_type;

    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    
    std::thread th([&reader]()
    {
      reader.start();
    });
    usleep(100000);
    write(dd[1], "test1", 5);
    usleep(50000);
    write(dd[1], "\r\n", 2);
    usleep(50000);
    write(dd[1], "test", 4);
    usleep(50000);
    write(dd[1], "2\r\n", 3);
    usleep(50000);
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
