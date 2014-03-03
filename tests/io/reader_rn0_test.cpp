#include <iostream>

#include <wfc/io/reader/reader.hpp>
#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/rn/reader/check/aspect.hpp>

/*
#include <wfc/io/strategy/posix/reader/rn/sync_read_return.hpp>
#include <wfc/io/strategy/posix/reader/rn/log.hpp>
#include <wfc/io/strategy/posix/reader/rn/trace.hpp>
#include <wfc/io/strategy/posix/reader/rn/incoming_buffer_check.hpp>
#include <wfc/io/strategy/posix/reader/rn/config.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
*/
#include <string>
#include <thread>
#include <boost/asio.hpp>


//typedef wfc::io::strategy::posix::reader::rn::config init_info;

struct reader_aspect: 
  fas::aspect<
    wfc::io::rn::reader::aspect2<wfc::io::reader::_incoming_>,
    wfc::io::reader::stream< boost::asio::posix::stream_descriptor, wfc::io::rn::reader::_incoming_>,
    wfc::io::reader::error_log
  >
{};
    

int main()
{
  
  size_t handler_count = 0;
  size_t not_alive_count = 0;
  std::function< void() > handler = nullptr;

  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  boost::asio::io_service::work wrk(*io_service);
  wfc::io::rn::reader::options init;
  init.input_buffer_size = 8096;
  init.not_alive = [&](){ ++not_alive_count;};
  
  {
    
    typedef wfc::io::reader::reader< reader_aspect > reader_type;
    
    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    handler = reader.wrap([&](){ ++handler_count;});
    
    std::thread th([&dd](){
      write(dd[1], "test1", 5);
      usleep(10000);
      write(dd[1], "\r\n", 2);
      usleep(10000);
      write(dd[1], "te", 2);
      usleep(10000);
      write(dd[1], "st2\r\n", 5);
      usleep(10000);
      write(dd[1], "test3\r\ntest4\r\n", 17);
    });
    
    std::vector<std::string> result;
    
    for (int i=0; i < 4; ++i)
    {
      auto d = reader.read();
      result.push_back( std::string(d->begin(), d->end()) );
    }
    th.join();
    if (result.size() != 4)
      abort();

    if (result[0] != "test1")
      abort();

    if (result[1] != "test2")
      abort();
    
    if (result[2] != "test3")
      abort();
    
    if (result[3] != "test4")
      abort();
    
    /*
    std::cout << "-1-" << std::endl;
    write(dd[1], "test1", 5);
    auto d = reader.read();
    if (d != nullptr)
      abort();
    std::cout << "-2-" << std::endl;
    write(dd[1], "\r\n", 2);
    d = reader.read();
    std::string result( d->begin(), d->end() );
    if ( result != "test1" )
      abort();
    std::cout << "-3-" << std::endl;
    write(dd[1], "te", 2);
    d = reader.read();
    if (d != nullptr)
      abort();
    std::cout << "-4-" << std::endl;
    write(dd[1], "st2\r\n", 5);
    d = reader.read();
    result.assign( d->begin(), d->end() );
    if ( result != "test2" )
      abort();
    */
    
    reader.get_io_service().post( handler );
    io_service->run_one();
    close(dd[0]);
    auto d = reader.read();
    if (d!=nullptr)
      abort();
    reader.get_io_service().post( handler );
  }
  
  // not posted last handler
  io_service->run_one();
  // execute not alive
  handler();
  
  if ( handler_count!=1 || not_alive_count!=2)
  {
    abort();
  }
}

