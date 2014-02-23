#include <iostream>

#include <wfc/io/reader/reader.hpp>
//#include <wfc/io/reader/read/async/aspect.hpp>

#include <wfc/io/reader/reader.hpp>
#include <wfc/io/strategy/posix/reader/async_read.hpp>
#include <wfc/io/strategy/posix/reader/trace.hpp>
#include <wfc/io/strategy/posix/reader/log.hpp>

#include <string>
#include <boost/asio.hpp>

struct init_info 
{
  std::shared_ptr<boost::asio::io_service> io_service;
  boost::asio::posix::stream_descriptor::native_handle_type native_handle;
  size_t input_buffer_size;
  std::function<void()> not_alive;
};

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
  init.io_service = io_service;
  init.native_handle = dd[0];
  init.input_buffer_size = 8096;
  init.not_alive = [&](){ ++not_alive_count;};
  
  
  {
    typedef fas::aspect<
      wfc::io::strategy::posix::reader::async_read,
      wfc::io::strategy::posix::reader::log,
      wfc::io::strategy::posix::reader::trace
    > async_read_ws_log;
    typedef wfc::io::reader::reader< async_read_ws_log > reader_type;
    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    
    handler = reader.wrap([&](){ ++handler_count;});
    
    write(dd[1], "test1", 5);    
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

    reader.read([&]( reader_type::data_ptr d )
    {
      ++test_count;
      std::string result( d->begin(), d->end() );
      if ( result != "test3" )
        abort();
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

/*
#include <iostream>

#include <wfc/io/reader/reader.hpp>
#include <wfc/io/reader/read/async/aspect.hpp>
#include <string>
#include <boost/asio.hpp>
#include <thread>
#include <csignal> 

void (*prev_handler)(int);

void my_handler (int )
{
  std::cout << "SIG" << std::endl;
  signal (SIGINT, prev_handler);
}

struct init_info 
{
  std::shared_ptr<boost::asio::io_service> io_service;
  boost::asio::posix::stream_descriptor::native_handle_type native_handle;
  size_t input_buffer_size;
  std::function<void()> not_alive;
};

int main()
{
  std::function<void()> wrp = nullptr;
  init_info init;
  auto io_service = std::make_shared<boost::asio::io_service>();
  init.io_service = io_service;
  init.input_buffer_size = 1024*8;
  init.not_alive = []() { std::cout << "not alive" << std::endl; };

  {
  int dd[2];
  ::pipe(dd);
  init.native_handle = dd[0];
  typedef wfc::io::reader::reader< wfc::io::reader::read::async::aspect > reader_type;
  reader_type reader(init);
  
  std::function<void()> zzz = [](){ std::cout << "wrapped" << std::endl;};
  wrp = reader.wrap(zzz);
  
  std::cout << "{{{" << std::endl;
  wrp();
  std::cout << "}}}" << std::endl;
  
  
  std::cout << "native_handle = " 
            <<  reader.get_aspect().get< wfc::io::_descriptor_ptr_>()->native_handle() << std::endl;
  
  std::string result;
  
  boost::asio::io_service::work wrk(*init.io_service);

  std::cout << "---" << std::endl;
  io_service->poll_one();
  std::cout << "---" << std::endl;
  io_service->poll_one();


  reader.async_read([&]( wfc::io::basic::data_ptr d){
    result.assign( d->begin(), d->end() );
    std::cout << "[" << result << "]"<< std::endl;
    if ( result != "hello" )
      abort();
  });

  
  reader.async_read([&]( wfc::io::basic::data_ptr d){
    result.assign( d->begin(), d->end() );
    std::cout << "[" << result << "]"<< std::endl;
    if ( result != "hello2" )
      abort();
  });
  
  std::cout << "-1-" << std::endl;
  io_service->poll_one();
  
  std::cout << "-2-" << std::endl;
  write(dd[1], "hello", 5);
  std::cout << "-3-" << std::endl;
  io_service->poll_one();
  std::cout << "-4-" << std::endl;
  write(dd[1], "hello2", 6);
  std::cout << "-5-" << std::endl;
  io_service->poll_one();
  std::cout << "-6-" << std::endl;
  
  write(dd[1], "hello3", 6);
  io_service->poll_one();
  reader.async_read([&]( reader_type::data_ptr d){
    result.assign( d->begin(), d->end() );
    std::cout << "[" << result << "]"<< std::endl;
    if ( result != "hello3" )
      abort();
  });
  io_service->poll_one();
  io_service->poll_one();
  io_service->poll_one();
  
  }
  wrp();
  
  return 0;
}
*/