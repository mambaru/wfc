#include <iostream>

/*
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/strategy/posix/reader/async_read_callback.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>
#include <wfc/io/strategy/posix/reader/config.hpp>
*/

#include <wfc/io/reader/reader.hpp>

#include <string>
#include <boost/asio.hpp>

//typedef wfc::io::strategy::posix::reader::config init_info;

/*
struct reader_aspect: 
  fas::aspect<
        fas::advice< wfc::io::_options_type_, init_info>,

    wfc::io::strategy::posix::reader::async_read_callback,
    wfc::io::strategy::posix::reader::log,
    wfc::io::strategy::posix::reader::trace
  >
{};
*/

struct reader_aspect
  : fas::aspect<
      wfc::io::reader::stream< boost::asio::posix::stream_descriptor >,
      wfc::io::reader::error_log
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
  wfc::io::reader::options init;
  init.input_buffer_size = 8096;
  init.not_alive = [&](){ ++not_alive_count; return wfc::callback_status::died;};
  
  {
    typedef wfc::io::reader::reader< reader_aspect > reader_type;
    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    
    handler = reader.owner().wrap([&](){ ++handler_count;}, [&](){++not_alive_count;} );
    
    write(dd[1], "test1", 5);    
    reader.async_read([&]( reader_type::data_ptr d )
    {
      ++test_count;
      std::string result( d->begin(), d->end() );
      std::cout << "result1 " << result << std::endl;
      if ( result != "test1" )
        abort();
      return wfc::callback_status::ready;
    });
    
    if ( test_count!=0 )
      std::abort();
    
    io_service->run_one();
    
    if ( test_count!=1 )
      std::abort();

    reader.async_read([&]( reader_type::data_ptr d )
    {
      ++test_count;
      std::string result( d->begin(), d->end() );
      if ( result != "test2" )
        abort();
      return wfc::callback_status::ready;
    });
    
    if ( test_count!=1 )
      std::abort();

    reader.async_read([&]( reader_type::data_ptr d )
    {
      ++test_count;
      std::string result( d->begin(), d->end() );
      if ( result != "test3" )
        abort();
      return wfc::callback_status::ready;
    });

    if ( test_count!=1 )
      std::abort();
    
    write(dd[1], "test2", 5);
    io_service->run_one();
    
    if ( test_count!=2 )
      std::abort();

    write(dd[1], "test3", 5);    
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

