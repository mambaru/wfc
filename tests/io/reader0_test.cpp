
#include <wfc/io/reader/reader.hpp>
//#include <wfc/io/reader/aspect.hpp>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
int main()
{
  /*
  boost::asio::io_service io_service;
  int dd[2];
  ::pipe(dd);
  wfc::io::posix::init init;
  init.io_service = &io_service;
  init.native_handle = dd[0];
  wfc::io::reader::reader< wfc::io::reader::aspect > reader( init );
 
  std::cout << reader.get_aspect().get< wfc::io::_descriptor_ptr_>()->native_handle() << std::endl;
  write(dd[1], "hello", 5);
  auto d = reader.read();
  std::string result( d->begin(), d->end() );
  std::cout << "[" << result << "]"<< std::endl;
  if ( result != "hello" )
    abort();
  
  write(dd[1], "hello2", 6);
  reader.async_read([](decltype(d) d ){ 
    std::string result( d->begin(), d->end() );
    std::cout << "[" << result << "]"<< std::endl;
  });
  io_service.run_one();
  */
  return 0;
}