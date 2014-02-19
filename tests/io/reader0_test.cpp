
#include <wfc/io/reader/reader.hpp>
#include <wfc/io/reader/read/sync/aspect.hpp>
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
  auto io_service = std::make_shared<boost::asio::io_service>();
  wfc::io::posix::init init;
  init.io_service = io_service;
  
  int dd[2];
  ::pipe(dd);
  init.native_handle = dd[0];
  wfc::io::reader::reader< wfc::io::reader::read::sync::aspect > reader;
  reader.initialize(init);
  std::cout << "native_handle = " 
            <<  reader.get_aspect().get< wfc::io::_descriptor_ptr_>()->native_handle() << std::endl;
  
  write(dd[1], "hello", 5);
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
  
  return 0;
}
