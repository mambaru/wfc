#include <iostream>

#include <wfc/logger/ilogger.hpp>
#include <wfc/core/global.hpp>
#include <wfc/callback/callback_status.hpp>

#include <wfc/io/reader/reader.hpp>
#include <string>
#include <boost/asio.hpp>




    

int main()
{
  std::cout << sizeof(boost::system::error_code) << std::endl;
  
  size_t handler_count = 0;
  size_t not_alive_count = 0;
  std::function< void() > handler = nullptr;

  int dd[2];
  ::pipe(dd);

  auto io_service = std::make_shared<boost::asio::io_service>();
  boost::asio::io_service::work wrk(*io_service);
  wfc::io::reader::options init;
  init.input_buffer_size = 8096;
  /*init.not_alive = [&]()
  {
    ++not_alive_count;
    return wfc::callback_status::died;
  };*/
  
  {
    
    typedef wfc::io::reader::reader< reader_aspect > reader_type;
    
    boost::asio::posix::stream_descriptor sd(*io_service, dd[0]);
    reader_type reader( std::move(sd), init);
    handler = reader.owner().wrap([&](){ ++handler_count;}, [&](){++not_alive_count;} );
    
    write(dd[1], "test1", 5);
    
    std::cout << "-read1.1-" << std::endl;
    auto d = reader.read();
    std::cout << "-read1.2-" << std::endl;
    std::string result( d->begin(), d->end() );
    if ( result != "test1" )
      abort();
    std::cout << "-read1.3-" << std::endl;
    
    write(dd[1], "test2", 5);
    d = reader.read();
    result.assign( d->begin(), d->end() );
    if ( result != "test2" )
      abort();
    
    reader.get_io_service().post( handler );
    io_service->run_one();
    close(dd[0]);
    
    if ( !reader.status() )
      abort();
    
    d = reader.read();
    
    if ( d!=nullptr )
      abort();

    if ( reader.status() )
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

