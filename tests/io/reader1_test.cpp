
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/reader/read/async/aspect.hpp>
//#include <wfc/io/reader/aspect.hpp>
#include <iostream>
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

int main()
{
  std::function<void()> wrp = nullptr;
  {
  auto io_service = std::make_shared<boost::asio::io_service>();
  wfc::io::posix::init init;
  init.io_service = io_service;
  
  int dd[2];
  ::pipe(dd);
  init.native_handle = dd[0];
  typedef wfc::io::reader::reader< wfc::io::reader::read::async::aspect > reader_type;
  reader_type reader;
  reader.initialize(init);
  
  std::function<void()> zzz = [](){ std::cout << "wrapped" << std::endl;};
  wrp = reader.wrap(zzz);
  wrp();
  
  /*
  boost::asio::io_service::strand strand(*io_service);
  auto xxx = strand.wrap(zzz);
  std::function<void()> wrp=[&xxx]{ xxx(); };
  */
  //reader.get_io_service().dispatch(zzz);
  
  std::cout << "native_handle = " 
            <<  reader.get_aspect().get< wfc::io::_descriptor_ptr_>()->native_handle() << std::endl;
  
  std::string result;
  
  boost::asio::io_service::work wrk(*io_service);

  io_service->poll_one();
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
  
  
  /*
  auto d = reader.read();
  std::string result( d->begin(), d->end() );
  std::cout << "[" << result << "]"<< std::endl;
  if ( result != "hello" )
    abort();
  
  prev_handler = signal (SIGINT, my_handler);
  std::thread([dd]()
  {
    std::cout << "-1-" << std::endl;
    ::sleep(1);
    std::cout << "-2-" << std::endl;
    raise(SIGINT);
    std::cout << "-3-" << std::endl;
    write(dd[1], "hello2", 6);
    std::cout << "-4-" << std::endl;
  }).detach();
  
  d = reader.read();
  result.assign( d->begin(), d->end() );
  std::cout << "[" << result << "]"<< std::endl;
  if ( result != "hello2" )
    abort();
  */
  }
  wrp();
  
  return 0;
}
