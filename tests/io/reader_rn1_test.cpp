#include <iostream>

#include <wfc/io/reader/reader.hpp>
#include <wfc/io/strategy/posix/reader/rn/async_read_callback.hpp>
#include <wfc/io/strategy/posix/reader/rn/log.hpp>
#include <wfc/io/strategy/posix/reader/rn/trace.hpp>
#include <wfc/io/strategy/posix/reader/rn/incoming_buffer_check.hpp>
#include <wfc/io/strategy/posix/reader/rn/config.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>

#include <string>
#include <boost/asio.hpp>

typedef wfc::io::strategy::posix::reader::rn::config init_info;

struct reader_aspect: 
  fas::aspect<
    wfc::io::strategy::posix::reader::trace,
    wfc::io::strategy::posix::reader::log,
    wfc::io::strategy::posix::reader::rn::trace,
    wfc::io::strategy::posix::reader::rn::log,
    wfc::io::strategy::posix::reader::rn::incoming_buffer_check,
    wfc::io::strategy::posix::reader::rn::async_read_callback
  >
{};


int main()
{
  size_t test_count = 0;
  size_t handler_count = 0;
  size_t not_alive_count = 0;
  std::function< void() > handler = nullptr;

  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  boost::asio::io_service::work wrk(*io_service);
  init_info init;
  init.input_buffer_size = 8096;
  init.not_alive = [&](){ ++not_alive_count;};
  
  {
    typedef wfc::io::reader::reader< reader_aspect > reader_type;
    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    
    handler = reader.wrap([&](){ ++handler_count;});
    
    write(dd[1], "test1\r\n", 7);    
    reader.read([&]( reader_type::data_ptr d )
    {
      ++test_count;
      std::string result( d->begin(), d->end() );
      if ( result != "test1" )
        abort();
    });
    
    if ( test_count!=0 )
      std::abort();
    
    io_service->run_one();
    
    if ( test_count!=1 )
      std::abort();

    reader.read([&]( reader_type::data_ptr d )
    {
      ++test_count;
      std::string result( d->begin(), d->end() );
      if ( result != "test2" )
        abort();
    });
    
    if ( test_count!=1 )
      std::abort();
    
    auto callback = [&]( reader_type::data_ptr d )
    {
      std::cout << "callback " << (d == nullptr) << std::endl;
      if ( d == nullptr )
        return;
      
      ++test_count;
      std::string result( d->begin(), d->end() );
      std::cout << "callback " << result << std::endl;
      if ( result != "test3" )
        abort();
    };

    reader.read(callback);
    reader.read(callback);
    reader.read(callback);

    if ( test_count!=1 )
      std::abort();
    
    write(dd[1], "test2\r\n", 7);
    io_service->run_one();
    
    if ( test_count!=2 )
      std::abort();

    write(dd[1], "t", 1);    
    io_service->run_one();
    write(dd[1], "es", 2);
    io_service->run_one();
    write(dd[1], "t3\r\n", 4);
    io_service->run_one();

    if ( test_count!=3 )
      std::abort();

    io_service->post( handler );
    io_service->run_one();
    io_service->post( handler );
  }
  
  // not posted last handler
  io_service->run_one();
  // execute not alive
  handler();
  
  if ( handler_count!=1 || not_alive_count!=2)
  {
    std::cout << handler_count << std::endl;
    std::cout << not_alive_count << std::endl;
    abort();
  }
}

